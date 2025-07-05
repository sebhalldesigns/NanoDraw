/***************************************************************
**
** NanoKit Library Source File
**
** File         :  nanodraw.c
** Module       :  nanodraw
** Author       :  SH
** Created      :  2025-07-05 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Drawing API
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include <nanodraw.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

#define FONT_CHARS_IN_ATLAS 256
#define FONT_CHAR_WIDTH_PX 8
#define FONT_CHAR_HEIGHT_PX 8
#define FONT_ATLAS_WIDTH_PX (FONT_CHARS_IN_ATLAS * FONT_CHAR_WIDTH_PX) // 256 * 8 = 2048
#define FONT_ATLAS_HEIGHT_PX FONT_CHAR_HEIGHT_PX // 8

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

/***************************************************************
** MARK: STATIC VARIABLES
***************************************************************/

extern const uint8_t shaders_glsl_shape_frag[];
extern const uint32_t shaders_glsl_shape_frag_size;

extern const uint8_t shaders_glsl_shape_vert[];
extern const uint32_t shaders_glsl_shape_vert_size;

extern const uint8_t shaders_glsl_texture_frag[];
extern const uint32_t shaders_glsl_texture_frag_size;

extern const uint8_t shaders_glsl_texture_vert[];
extern const uint32_t shaders_glsl_texture_vert_size;

extern const uint8_t console_font_8x8[];

static char log[512];


// Create a temporary buffer on the stack to hold the unpacked pixel data.
unsigned char unpacked_font_data[FONT_ATLAS_WIDTH_PX * FONT_CHAR_HEIGHT_PX];

/***************************************************************
** MARK: STATIC FUNCTION DEFS
***************************************************************/

static GLuint CreateShader(const char* vertSrc, const char* fragSrc);
static void FlushContext(nkDrawContext_t* ctx);

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkDrawContext_Create(nkDrawContext_t *context)
{
    context->ShapeShaderProgram = CreateShader(
        (const char*)shaders_glsl_shape_vert, 
        (const char*)shaders_glsl_shape_frag
    );

    if (context->ShapeShaderProgram == 0) 
    {
        fprintf(stderr, "ERROR: Failed to create shape shader program.\n");
        return false;
    }

    context->TextureShaderProgram = CreateShader(
        (const char*)shaders_glsl_texture_vert, 
        (const char*)shaders_glsl_texture_frag
    );

    if (context->TextureShaderProgram == 0) 
    {
        fprintf(stderr, "ERROR: Failed to create texture shader program.\n");
        glDeleteProgram(context->ShapeShaderProgram);
        return false;
    }

     // Get uniform locations
    context->ShapeProjMatLoc = glGetUniformLocation(context->ShapeShaderProgram, "uProjection");
    context->TextureProjMatLoc = glGetUniformLocation(context->TextureShaderProgram, "uProjection");
    GLint textureSamplerLoc = glGetUniformLocation(context->TextureShaderProgram, "uTexture");
    // Set the texture sampler uniform once, since it will always be texture unit 0
    glUseProgram(context->TextureShaderProgram);
    glUniform1i(textureSamplerLoc, 0);
    glUseProgram(0);


    // --- 3. Create Vertex Buffer and Vertex Array Object ---
    glGenVertexArrays(1, &context->VAO);
    glGenBuffers(1, &context->VBO);

    glBindVertexArray(context->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);

    // Allocate the buffer on the GPU. We pass NULL for the data because we're not uploading
    // anything yet, but we give it the full size. GL_DYNAMIC_DRAW is a hint that we'll
    // be updating this buffer's content frequently.
    glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE * sizeof(nkVertex_t), NULL, GL_DYNAMIC_DRAW);

    // Specify the layout of our ndVertex_t struct
    // Position attribute (2 floats)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(nkVertex_t), (void*)offsetof(nkVertex_t, x));
    glEnableVertexAttribArray(0);
    // Color attribute (4 floats)
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(nkVertex_t), (void*)offsetof(nkVertex_t, r));
    glEnableVertexAttribArray(1);
    // Texture coordinate attribute (2 floats)
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(nkVertex_t), (void*)offsetof(nkVertex_t, u));
    glEnableVertexAttribArray(2);
    
    // Unbind to prevent accidental modification
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // For a working example, you would need to fill this bitmap.
    // Let's create a placeholder checkerboard pattern for demonstration.
    glGenTextures(1, &context->FontTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, context->FontTexture);
    
    // Use GL_NEAREST for pixel-perfect rendering of the font
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Loop for each of the 8 rows (scanlines) of the final texture
    for (int y = 0; y < FONT_CHAR_HEIGHT_PX; ++y) {
        // Loop for each of the 256 characters in the font atlas
        for (int char_index = 0; char_index < FONT_CHARS_IN_ATLAS; ++char_index) {
            
            // Get the single packed byte for this character at its 'y'th row.
            // In console_font_8x8, each character takes 8 bytes, so we jump by 8.
            unsigned char packed_byte = console_font_8x8[(char_index * FONT_CHAR_HEIGHT_PX) + y];
            
            // Unpack the 8 pixels (the 8 bits) from this byte
            for (int x_bit = 0; x_bit < FONT_CHAR_WIDTH_PX; ++x_bit) {
                
                // Calculate the destination pixel's X and Y coordinate in the final texture
                int dest_x = (char_index * FONT_CHAR_WIDTH_PX) + x_bit;
                int dest_y = y;
                
                // Calculate the destination index in the 1D unpacked_font_data array
                int dest_index = dest_y * FONT_ATLAS_WIDTH_PX + dest_x;
                
                // Check the most significant bit (the leftmost pixel) first
                if ((packed_byte & 0x80) != 0) {
                    unpacked_font_data[dest_index] = 0xFF; // White pixel
                } else {
                    unpacked_font_data[dest_index] = 0x00; // Black pixel
                }
                // Shift the bits to the left to process the next one in the next loop iteration
                packed_byte <<= 1;
            }
        }
    }
    // --- END CORRECTED UNPACKING LOOP ---
    
    
    // We upload the bitmap as a grayscale (1-channel) texture.
     // Now, upload the correctly formatted UNPACKED data to OpenGL.
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 
                 FONT_ATLAS_WIDTH_PX, FONT_ATLAS_HEIGHT_PX, 
                 0, GL_RED, GL_UNSIGNED_BYTE, 
                 unpacked_font_data); // Use the new unpacked buffer
    
    // Use swizzling to make the single red channel act like an alpha mask.
    // The shader will see texture() -> vec4(1.0, 1.0, 1.0, R). We can then multiply by color.
    // Or, more simply, we can make it vec4(R, R, R, R) and multiply.
    GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- 5. Set Initial State ---
    context->VertexCount = 0;
    context->CurrentDrawMode = GL_TRIANGLES; // Default to triangles
    context->CurrentShader = context->ShapeShaderProgram;
    nkDrawContext_SetColor(context, (nkVector4_t){1.0f, 1.0f, 1.0f, 1.0f}); // Default color white
    context->FontCharWidth = 8;
    context->FontCharHeight = 8;

    printf("NanoDraw context created successfully.\n");
    return true;
}

void nkDrawContext_BeginFrame(nkDrawContext_t *context, float width, float height)
{

    context->VertexCount = 0; // Reset vertex count for the new frame

    /* set to invalid values to force a state change */
    context->CurrentDrawMode = 0; 
    context->CurrentShader = 0;

    const float L = 0.0f;
    const float R = width;
    const float B = height;
    const float T = 0.0f;

    // The orthographic projection matrix:
    // [ 2/(R-L)     0           0       -(R+L)/(R-L) ]
    // [ 0           2/(T-B)     0       -(T+B)/(T-B) ]
    // [ 0           0           -1      0            ]
    // [ 0           0           0       1            ]
    const float projection[4][4] = {
        { 2.0f / (R - L),   0.0f,               0.0f,   0.0f },
        { 0.0f,             2.0f / (T - B),     0.0f,   0.0f },
        { 0.0f,             0.0f,               -1.0f,  0.0f },
        { -(R + L)/(R - L), -(T + B)/(T - B),   0.0f,   1.0f }
    };

    // Upload this matrix as a uniform to both of our shader programs.
    // We do it for both because we don't know which one will be used first.
    glUseProgram(context->ShapeShaderProgram);
    glUniformMatrix4fv(context->ShapeProjMatLoc, 1, GL_FALSE, &projection[0][0]);

    glUseProgram(context->TextureShaderProgram);
    glUniformMatrix4fv(context->TextureProjMatLoc, 1, GL_FALSE, &projection[0][0]);

    // Unbind the program to be tidy.
    glUseProgram(0);


}

void nkDrawContext_EndFrame(nkDrawContext_t *context)
{
    // If we have any vertices to draw, flush them.
    if (context->VertexCount > 0) {
        FlushContext(context);
    }

    // Reset the current state
    context->CurrentDrawMode = 0;
    context->CurrentShader = 0;

    glBindVertexArray(0);
    glUseProgram(0);

}

void nkDrawContext_SetColor(nkDrawContext_t *context, nkVector4_t color)
{
    context->CurrentColor[0] = color.r;
    context->CurrentColor[1] = color.g;
    context->CurrentColor[2] = color.b;
    context->CurrentColor[3] = color.a;
}

float nkDrawContext_DrawText(nkDrawContext_t* context, const char* text, float x, float y, float scale)
{
    // --- 1. State Management: Prepare for textured drawing ---
    const int vertices_per_char = 6;
    
    if (context->CurrentDrawMode != GL_TRIANGLES || context->CurrentShader != context->TextureShaderProgram) {
        FlushContext(context);
    }
    
    context->CurrentDrawMode = GL_TRIANGLES;
    context->CurrentShader = context->TextureShaderProgram;
    
    // --- 2. Prepare for drawing ---
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, context->FontTexture);

    const float scaledWidth = (float)context->FontCharWidth * scale;
    const float scaledHeight = (float)context->FontCharHeight * scale;
    float penX = x;

    const float r = context->CurrentColor[0];
    const float g = context->CurrentColor[1];
    const float b = context->CurrentColor[2];
    const float a = context->CurrentColor[3];
    
    // This is correct now, as your texture atlas contains 256 characters.
    const float atlasWidthInChars = 256.0f;

    // --- 3. Iterate through text and generate vertices ---
    // Cast to unsigned char* to correctly handle ASCII values from 0-255.
    for (const unsigned char* p = (const unsigned char*)text; *p; ++p) {
        unsigned char c = *p;
        
        if (context->VertexCount + vertices_per_char > VERTEX_BUFFER_SIZE) {
            FlushContext(context);
            glBindTexture(GL_TEXTURE_2D, context->FontTexture);
        }

        const float x0 = penX;
        const float y0 = y;
        const float x1 = penX + scaledWidth;
        const float y1 = y + scaledHeight;

        // ***** FIX IS HERE *****
        // The character's index is its own ASCII value because the font starts at 0.
        const int charIndex = c;
        // The UV calculation logic itself is correct for your atlas layout.
        const float u0 = (float)charIndex / atlasWidthInChars;
        const float u1 = (float)(charIndex + 1) / atlasWidthInChars;
        // *********************

        const float v0 = 0.0f;
        const float v1 = 1.0f;

        nkVertex_t* v = &context->VertexBuffer[context->VertexCount];

        v[0] = (nkVertex_t){ x0, y0, r, g, b, a, u0, v0 };
        v[1] = (nkVertex_t){ x1, y0, r, g, b, a, u1, v0 };
        v[2] = (nkVertex_t){ x0, y1, r, g, b, a, u0, v1 };
        
        v[3] = (nkVertex_t){ x1, y0, r, g, b, a, u1, v0 };
        v[4] = (nkVertex_t){ x1, y1, r, g, b, a, u1, v1 };
        v[5] = (nkVertex_t){ x0, y1, r, g, b, a, u0, v1 };
        
        context->VertexCount += vertices_per_char;
        
        penX += scaledWidth;
    }

    return penX - x;
}

/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/

static GLuint CreateShader(const char* vertSrc, const char* fragSrc)
{
    GLuint vertShader, fragShader, program;
    GLint status;

    /* Compile vertex shader */
    vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertSrc, NULL);
    glCompileShader(vertShader);
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) 
    {
        glGetShaderInfoLog(vertShader, 512, NULL, log);
        fprintf(stderr, "ERROR: Vertex shader compilation failed:\n%s\n", log);
        glDeleteShader(vertShader);
        return 0;
    }

    /* Compile fragment shader */
    fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragSrc, NULL);
    glCompileShader(fragShader);
    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE) 
    {
        glGetShaderInfoLog(fragShader, 512, NULL, log);
        fprintf(stderr, "ERROR: Fragment shader compilation failed:\n%s\n", log);
        glDeleteShader(vertShader);
        glDeleteShader(fragShader);
        return 0;
    }

    /* Link shaders into a program */
    program = glCreateProgram();
    glAttachShader(program, vertShader);
    glAttachShader(program, fragShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE) 
    {
        glGetProgramInfoLog(program, 512, NULL, log);
        fprintf(stderr, "ERROR: Shader program linking failed:\n%s\n", log);
        glDeleteProgram(program);
        program = 0;
    }

    /* Shaders are now linked into the program and are no longer needed */
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    return program;
}

static void FlushContext(nkDrawContext_t* context) {
    if (context->VertexCount == 0) {
        return;
    }

    glUseProgram(context->CurrentShader);
    // Bind textures if using texture shader, set uniforms etc.
    
    glBindVertexArray(context->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, context->VertexCount * sizeof(nkVertex_t), context->VertexBuffer);

    // Use the primitive mode that was being batched
    glDrawArrays(context->CurrentDrawMode, 0, context->VertexCount);

    // Reset the counter for the next batch
    context->VertexCount = 0;
}