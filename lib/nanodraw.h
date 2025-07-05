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

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#define VERTEX_BUFFER_SIZE (255U)

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

typedef struct
{
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

typedef struct nkDrawContext_t
{
    /* persistent */
    GLuint ShapeShaderProgram;
    GLuint TextureShaderProgram;
    GLuint VAO;
    GLuint VBO;
    GLuint FontTexture;

    nkVertex_t VertexBuffer[VERTEX_BUFFER_SIZE];
    
    size_t VertexCount;
    GLenum CurrentDrawMode;
    GLuint CurrentTexture;
    GLuint CurrentShader;

    float CurrentColor[4];

    uint32_t ShapeProjMatLoc;
    uint32_t TextureProjMatLoc;

    uint32_t FontCharWidth;
    uint32_t FontCharHeight;


} nkDrawContext_t;

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkDrawContext_Create(nkDrawContext_t *context); 

void nkDrawContext_BeginFrame(nkDrawContext_t *context, float width, float height);
void nkDrawContext_EndFrame(nkDrawContext_t *context);

void nkDrawContext_SetColor(nkDrawContext_t *context, nkVector4_t color);

float nkDrawContext_DrawText(nkDrawContext_t* context, const char* text, float x, float y, float scale);

#ifdef __cplusplus
}
#endif

#endif /* NANODRAW_H */
