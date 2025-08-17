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
            nvgFontSize(context->nvgContext, 14.0f);
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

/* angle in radians, clockwise from vertical */
void nkDraw_SetColorGradient(nkDrawContext_t *context, nkVector4_t colorStart, nkVector4_t colorEnd, float angle, float x, float y, float w, float h)
{
    NVGcolor start = nvgRGBAf(colorStart.r, colorStart.g, colorStart.b, colorStart.a);
    NVGcolor end = nvgRGBAf(colorEnd.r, colorEnd.g, colorEnd.b, colorEnd.a);

    float dx = sinf(angle);
    float dy = cosf(angle);

    float halfDiag = 0.5f * (fabsf(dx) * w + fabsf(dy) * h);

    float cx = x + w * 0.5f;
    float cy = y + h * 0.5f;

    float x0 = cx - dx * halfDiag;
    float y0 = cy - dy * halfDiag;
    float x1 = cx + dx * halfDiag;
    float y1 = cy + dy * halfDiag;

    NVGpaint paint = nvgLinearGradient(
        context->nvgContext, 
        x0, y0, x1, y1,
        start, 
        end
    );
    
    nvgFillPaint(context->nvgContext, paint);
    
}

void nkDraw_SetStrokeColor(nkDrawContext_t *context, nkVector4_t color)
{
    nvgStrokeColor(context->nvgContext, nvgRGBAf(color.r, color.g, color.b, color.a));

}

void nkDraw_SetStrokeColorGradient(nkDrawContext_t *context, nkVector4_t colorStart, nkVector4_t colorEnd, float angle, float x, float y, float w, float h)
{
    NVGcolor start = nvgRGBAf(colorStart.r, colorStart.g, colorStart.b, colorStart.a);
    NVGcolor end = nvgRGBAf(colorEnd.r, colorEnd.g, colorEnd.b, colorEnd.a);

    float dx = sinf(angle);
    float dy = cosf(angle);

    float halfDiag = 0.5f * (fabsf(dx) * w + fabsf(dy) * h);

    float cx = x + w * 0.5f;
    float cy = y + h * 0.5f;

    float x0 = cx - dx * halfDiag;
    float y0 = cy - dy * halfDiag;
    float x1 = cx + dx * halfDiag;
    float y1 = cy + dy * halfDiag;

    NVGpaint paint = nvgLinearGradient(
        context->nvgContext, 
        x0, y0, x1, y1,
        start, 
        end
    );
    
    nvgStrokePaint(context->nvgContext, paint);
}

void nkDraw_SetStrokeWidth(nkDrawContext_t *context, float width)
{
    nvgStrokeWidth(context->nvgContext, width);
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

void nkDraw_RoundedRect(nkDrawContext_t* context, float x, float y, float w, float h, float radius)
{
    nvgBeginPath(context->nvgContext);
    nvgRoundedRect(context->nvgContext, x, y, w, h, radius);
    
    nvgFill(context->nvgContext);
}

void nkDraw_RoundedRectPath(nkDrawContext_t* context, float x, float y, float w, float h, float radius)
{
    nvgBeginPath(context->nvgContext);
    nvgRoundedRect(context->nvgContext, x, y, w, h, radius);
    
    nvgStroke(context->nvgContext);
}

nkRect_t nkDraw_MeasureText(nkDrawContext_t* context, nkFont_t* font, const char* text)
{
    float bounds[4] = {0};
    
    nvgFontSize(context->nvgContext, 14.0f);
    nvgFontFace(context->nvgContext, "sans");

    nvgTextBounds(context->nvgContext, 0.0f, 0.0f, text, NULL, bounds);

    return (nkRect_t) {
        .x = bounds[0],
        .y = bounds[1],
        .width = bounds[2] - bounds[0],
        .height = bounds[3] - bounds[1]
    };
}

/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/
