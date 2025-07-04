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

bool nkDraw_CreateContext(nkDrawContext_t *context)
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

    // Allocate the buffer on the GPU. We pass NULL for the data because we're not uploading
    // anything yet, but we give it the full size. GL_DYNAMIC_DRAW is a hint that we'll
    // be updating this buffer's content frequently.
    glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE * sizeof(nkVertex_t), NULL, GL_DYNAMIC_DRAW);

    
    // Unbind to prevent accidental modification
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // --- 5. Set Initial State ---
    context->VertexCount = 0;
    context->CurrentDrawMode = GL_TRIANGLES; // Default to triangles
    context->CurrentShader = context->ShapeShaderProgram;

    nkDraw_SetColor(context, (nkVector4_t){1.0f, 1.0f, 1.0f, 1.0f}); // Default color white

    printf("NanoDraw context created successfully.\n");
    return true;
}

void nkDraw_Begin(nkDrawContext_t *context, float width, float height)
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

void nkDraw_End(nkDrawContext_t *context)
{
    // If we have any vertices to draw, flush them.
    if (context->VertexCount > 0) {
        FlushContext(context);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void nkDraw_SetColor(nkDrawContext_t *context, nkVector4_t color)
{
    context->CurrentColor[0] = color.r;
    context->CurrentColor[1] = color.g;
    context->CurrentColor[2] = color.b;
    context->CurrentColor[3] = color.a;
}

float nkDraw_Text(nkDrawContext_t* context, nkFont_t* font, const char* text, float x, float y, float scale)
{

    if (
            (context->CurrentDrawMode != GL_TRIANGLES) 
        ||  (context->CurrentShader != context->TextureShaderProgram)
    ) 
    {
        FlushContext(context);
    }
    
    context->CurrentDrawMode = GL_TRIANGLES;
    context->CurrentShader = context->TextureShaderProgram;
    
    // --- 2. Prepare for drawing ---
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, font->AtlasTexture);

    float pen_x = x;
    float pen_y = y;

    const float r = context->CurrentColor[0];
    const float g = context->CurrentColor[1];
    const float b = context->CurrentColor[2];
    const float a = context->CurrentColor[3];
    
    for (const char* p = text; *p; ++p) {
        // Only process printable ASCII characters that we have baked
        if (*p >= 32 && *p < 128) {
            
            stbtt_aligned_quad q;
            
            // This function calculates the vertex positions and UVs for the character quad
            // It also advances pen_x and pen_y for you, including kerning.
            stbtt_GetBakedQuad(font->BakedCharData, 
                               font->Width, font->Height, 
                               *p - 32, // Character index (0-95)
                               &pen_x, &pen_y, 
                               &q, 1); // 1 for opengl texture coordinates

            // Check for buffer overflow
            if (context->VertexCount + 6 > VERTEX_BUFFER_SIZE) {
                FlushContext(context);
                glBindTexture(GL_TEXTURE_2D, font->AtlasTexture);
            }

            nkVertex_t* v = &context->VertexBuffer[context->VertexCount];

            v[0] = (nkVertex_t){ q.x0, q.y0, r, g, b, a, q.s0, q.t0 };
            v[1] = (nkVertex_t){ q.x1, q.y0, r, g, b, a, q.s1, q.t0 };
            v[2] = (nkVertex_t){ q.x0, q.y1, r, g, b, a, q.s0, q.t1 };

            v[3] = (nkVertex_t){ q.x1, q.y0, r, g, b, a, q.s1, q.t0 };
            v[4] = (nkVertex_t){ q.x1, q.y1, r, g, b, a, q.s1, q.t1 };
            v[5] = (nkVertex_t){ q.x0, q.y1, r, g, b, a, q.s0, q.t1 };
            
            context->VertexCount += 6;
        }
    }
    
    return pen_x - x; // Return total width
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

static void FlushContext(nkDrawContext_t* context) 
{

    if (context->VertexCount == 0) 
    {
        return;
    }

    
    glBindBuffer(GL_ARRAY_BUFFER, context->VBO);
    glBufferData(GL_ARRAY_BUFFER, VERTEX_BUFFER_SIZE * sizeof(nkVertex_t), NULL, GL_DYNAMIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, context->VertexCount * sizeof(nkVertex_t), context->VertexBuffer);

    // Use the primitive mode that was being batched
    glUseProgram(context->CurrentShader);
    glBindVertexArray(context->VAO);
    glDrawArrays(context->CurrentDrawMode, 0, context->VertexCount);

    //printf("Drawing %zu vertices with mode %d using shader %d\n", context->VertexCount, context->CurrentDrawMode, context->CurrentShader);

    // Reset the counter for the next batch
    context->VertexCount = 0;
}