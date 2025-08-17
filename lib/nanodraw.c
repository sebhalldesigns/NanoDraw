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

#if __EMSCRIPTEN__
    #define NANOVG_GLES3_IMPLEMENTATION
#else 
    #define NANOVG_GL3_IMPLEMENTATION
#endif

#include <extern/nanovg/nanovg_gl.h>


/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

/***************************************************************
** MARK: STATIC VARIABLES
***************************************************************/


extern const uint8_t NKFonts_fonts_Roboto_Regular_ttf[];
extern const size_t NKFonts_fonts_Roboto_Regular_ttf_size;


/***************************************************************
** MARK: STATIC FUNCTION DEFS
***************************************************************/

/***************************************************************
** MARK: PUBLIC FUNCTIONS
***************************************************************/

bool nkDraw_CreateContext(nkDrawContext_t *context)
{

    #if __EMSCRIPTEN__
        context->nvgContext = nvgCreateGLES3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    #else
        context->nvgContext = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    #endif

    if (!context->nvgContext) 
    {
        fprintf(stderr, "ERROR: Failed to create NanoVG context.\n");
    }
    else 
    {
        int font = nvgCreateFontMem(context->nvgContext, "sans", NKFonts_fonts_Roboto_Regular_ttf, NKFonts_fonts_Roboto_Regular_ttf_size, 0);
        if (font == -1)
        {
            fprintf(stderr, "ERROR: Failed to load font into NanoVG context.\n");
        }
        else 
        {
            printf("Font loaded successfully with ID: %d\n", font);
            nvgFontSize(context->nvgContext, 16.0f);
            nvgFontFace(context->nvgContext, "sans");
        }

        printf("NanoDraw context created successfully.\n");
    }


    return context->nvgContext != NULL;
}

void nkDraw_Begin(nkDrawContext_t *context, float width, float height)
{
    nvgBeginFrame(context->nvgContext, width, height, 1.0f);
}

void nkDraw_End(nkDrawContext_t *context)
{
    nvgEndFrame(context->nvgContext);
}

void nkDraw_SetColor(nkDrawContext_t *context, nkVector4_t color)
{

    nvgFillColor(context->nvgContext, nvgRGBAf(color.r, color.g, color.b, color.a));

}

void nkDraw_Text(nkDrawContext_t* context, nkFont_t* newFont, const char* text, float x, float y)
{
    nvgBeginPath(context->nvgContext);
    
    nvgFontSize(context->nvgContext, 14.0f);
    nvgFontFace(context->nvgContext, "sans");

    nvgText(context->nvgContext, x, y, text, NULL);
}

void nkDraw_Rect(nkDrawContext_t* context, float x, float y, float w, float h)
{
    nvgBeginPath(context->nvgContext);
    nvgRect(context->nvgContext, x, y, w, h);
    nvgFill(context->nvgContext);
}

/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/
