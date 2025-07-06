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

#include <extern/glad/glad.h>
#include <extern/stb/stb_truetype.h>

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define VERTEX_BUFFER_SIZE (1024*1024U)

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

typedef struct
{
    uint32_t vertexType; // 0 for shape, 1 for texture
    float x, y;
    float r, g, b, a;
    float u, v;
} nkVertex_t;

typedef struct 
{   
    union
    {
        float r;
        float x;
    };

    union
    {
        float g;
        float y;
    };

    union
    {
        float b;
        float z;
    };

    union
    {
        float a;
        float w;
    };
    
} nkVector4_t;

typedef struct
{
    /* persistent */
    GLuint GeneralShaderProgram;
    GLuint VAO;
    GLuint VBO;

    nkVertex_t VertexBuffer[VERTEX_BUFFER_SIZE];
    size_t VertexCount;

    GLenum CurrentDrawMode;
    GLuint CurrentTexture;
    GLuint CurrentShader;
    float CurrentColor[4];

    uint32_t GeneralProjMatLoc;


} nkDrawContext_t;

typedef struct 
{
    stbtt_bakedchar BakedCharData[96]; /* ASCII 32-126 */

    GLuint AtlasTexture;
    size_t Width;
    size_t Height;

    float FontSize;
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

#ifdef __cplusplus
}
#endif

#endif /* NANODRAW_H */
