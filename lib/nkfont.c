/***************************************************************
**
** NanoKit Library Source File
**
** File         :  nkfont.c
** Module       :  nanodraw
** Author       :  SH
** Created      :  2025-07-05 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Font API
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#define STB_TRUETYPE_IMPLEMENTATION
#include <nanodraw.h>
#include <stdlib.h>
#include <stdio.h>

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

/***************************************************************
** MARK: STATIC VARIABLES
***************************************************************/

/***************************************************************
** MARK: STATIC FUNCTION DEFS
***************************************************************/

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkFont_Load(nkFont_t *font, const char *filename, float fontSize, uint8_t *atlas_buffer, size_t atlas_buffer_width, size_t atlas_buffer_height)
{
    uint8_t* ttf_buffer = NULL;
    size_t file_size = 0;
    FILE* fontFile = fopen(filename, "rb");

    if (!fontFile) 
    {
        fprintf(stderr, "ERROR: Failed to open font file '%s'\n", filename);
        return false;
    }

    fseek(fontFile, 0, SEEK_END);
    file_size = ftell(fontFile);
    fseek(fontFile, 0, SEEK_SET);

    ttf_buffer = (unsigned char*)malloc(file_size);

    if (!ttf_buffer || fread(ttf_buffer, 1, file_size, fontFile) != file_size) 
    {
        fprintf(stderr, "ERROR: Failed to read font file into buffer.\n");
        fclose(fontFile);
        free(ttf_buffer);
        return false;
    }

    fclose(fontFile);

    /* load font here */
    int result = stbtt_BakeFontBitmap(
        ttf_buffer, 
        0, 
        fontSize, 
        atlas_buffer, 
        atlas_buffer_width, 
        atlas_buffer_height,                               
        32, 96, 
        font->BakedCharData
    );

    free(ttf_buffer);

    if (result <= 0) 
    {
        fprintf(stderr, "ERROR: Failed to bake font bitmap. Atlas may be too small for font size %.2f.\n", fontSize);
        return false;
    }

    glGenTextures(1, &font->AtlasTexture);
    glBindTexture(GL_TEXTURE_2D, font->AtlasTexture);

    // Use GL_LINEAR filtering for smooth rendering if the text is scaled.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Tell OpenGL how to unpack the pixel data
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    // Upload the 1-channel (grayscale) bitmap data.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlas_buffer_width, atlas_buffer_height,
                 0, GL_RED, GL_UNSIGNED_BYTE, atlas_buffer);

    GLint swizzleMask[] = {GL_ONE, GL_ONE, GL_ONE, GL_RED};
    glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzleMask);

    // --- Step 4: Store final font data in the struct ---
    font->Width = atlas_buffer_width;
    font->Height = atlas_buffer_height;
    font->FontSize = fontSize;

    printf("Font '%s' loaded successfully with size %.2f.\n", filename, fontSize);

    return true;
}

/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/
