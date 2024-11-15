#ifndef NANODRAW_H
#define NANODRAW_H


/******************************************************************************
 * NanoWin.c
 * 
 * NanoWin is a simple windowing library built on top of SDL2. It provides a 
 * simple API for creating windows and handling events.
 * 
 * November 2024, Seb Hall
 *****************************************************************************/

#ifndef NANOWIN_H
typedef struct nWindow* nWindow_h;
#endif // NANOWIN_H

typedef struct nDrawingContext* nDrawingContext_h;

typedef struct nTypeface* nTypeface_h;

typedef struct
{
    float r;
    float g;
    float b;
    float a;
} nDrawColor;

typedef enum 
{
    SET_FILL_COLOR,
    SET_TYPEFACE,
    SET_ORIGIN,
    SET_SIZE,
    SET_TEXT,
    
    DRAW_TEXT,
    DRAW_RECT
} nDrawCommand;

typedef union 
{

    struct 
    {
        nDrawColor color;
    } fillColorData;

    struct 
    {
        nTypeface_h typeface;
    } typefaceData;

    struct 
    {
        float x;
        float y;
    } originData;

    struct 
    {
        float width;
        float height;
    } sizeData;

    struct 
    {
        const char* text;
    } textData;

} nDrawCommandData;



nDrawingContext_h NanoDraw_SetupForWindow(nWindow_h window, void *sdlWindow, void *sdlRenderer);

nTypeface_h NanoDraw_CreateTypeface(nDrawingContext_h context, const char* typeface, float size);

void NanoDraw_BeginFrame(nDrawingContext_h context);
void NanoDraw_EndFrame(nDrawingContext_h context);

void NanoDraw_Draw(nDrawingContext_h context, nDrawCommand command, nDrawCommandData data);




#endif // NANODRAW_H


