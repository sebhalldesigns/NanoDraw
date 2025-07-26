/***************************************************************
**
** NanoKit Library Header File
**
** File         :  nanodraw.h
** Module       :  nanodraw
** Author       :  SH
** Created      :  2025-07-05 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Drawing API
**
***************************************************************/

#ifndef NANODRAW_H
#define NANODRAW_H

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#if __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include <extern/glad/glad.h>
#endif 

#include <extern/stb/stb_truetype.h>

#include "color.h"
#include "geometry.h"

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define VERTEX_BUFFER_SIZE  (1024*1024U)
#define TEXTURE_ATTACHMENTS (16U)

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

typedef struct
{
    uint32_t vertexType;
    float x, y;
    float r, g, b, a;
    float u, v;
} nkVertex_t;

typedef struct
{
    /* persistent */
    GLuint generalShaderProgram;
    GLuint vao;
    GLuint vbo;

    nkVertex_t vertexBuffer[VERTEX_BUFFER_SIZE];
    size_t vertexCount;

    GLenum currentDrawMode;
    GLuint currentTexture;
    GLuint currentShader;
    float currentColor[4];

    uint32_t generalProjMatLoc;
} nkDrawContext_t;

typedef struct 
{
    stbtt_bakedchar bakedCharData[96]; /* ASCII 32-126 */

    GLuint atlasTexture;
    size_t width;
    size_t height;

    float fontSize;
} nkFont_t;



/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkDraw_CreateContext(nkDrawContext_t *context); 
void nkDraw_Begin(nkDrawContext_t *context, float width, float height);
void nkDraw_End(nkDrawContext_t *context);

void nkDraw_SetColor(nkDrawContext_t *context, nkVector4_t color);

void nkDraw_Text(nkDrawContext_t* context, nkFont_t* font, const char* text, float x, float y);
void nkDraw_Rect(nkDrawContext_t* context, float x, float y, float w, float h);

bool nkFont_Load(nkFont_t *font, const char *filename, float fontSize, uint8_t *atlas_buffer, size_t atlas_buffer_width, size_t atlas_buffer_height);
bool nkFont_LoadFromMemory(nkFont_t *font, uint8_t *data, size_t dataSize, float fontSize, uint8_t *atlas_buffer, size_t atlas_buffer_width, size_t atlas_buffer_height);

#ifdef __cplusplus
}
#endif

#endif /* NANODRAW_H */
