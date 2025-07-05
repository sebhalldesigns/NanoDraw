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

static char log[512];

/***************************************************************
** MARK: STATIC FUNCTION DEFS
***************************************************************/

static GLuint CreateShader(const char* vertSrc, const char* fragSrc);

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