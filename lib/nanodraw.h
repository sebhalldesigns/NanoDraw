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

#define VERTEX_BUFFER_SIZE (65536U)

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

typedef struct
{
    float x, y;
    float r, g, b, a;
    float u, v;
} nkVertex_t;

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

    float CurrentColor[4];

} nkDrawContext_t;

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

bool nkDrawContext_Create(nkDrawContext_t *context); 


#ifdef __cplusplus
}
#endif

#endif /* NANODRAW_H */
