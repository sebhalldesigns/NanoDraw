
/******************************************************************************
 * NanoWin.c
 * 
 * NanoWin is a simple windowing library built on top of SDL2. It provides a 
 * simple API for creating windows and handling events.
 * 
 * November 2024, Seb Hall
 *****************************************************************************/

extern "C" {
    #include "NanoDraw.h"
}

#include <SDL2/SDL.h>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/backends/imgui_impl_sdl2.h>
#include <imgui/backends/imgui_impl_sdlrenderer2.h>
    
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <map>
#include <vector>

/******************************************************************************
 * MARK: TYPE DEFINITIONS
 *****************************************************************************/

typedef struct nTypeface
{
    const char* typeface;
    float size;
    ImFont* imguiFont;
} nTypeface;

typedef struct nDrawingState 
{
    nDrawColor fillColor;
    float x;
    float y;
    float width;
    float height;
    const char* text = NULL;
    nTypeface_h typeface = NULL;
} nDrawingState;



typedef struct nDrawingContext 
{
    nDrawingState state;

    ImGuiContext* imguiContext;
    ImDrawList* imguiDrawList;
    std::vector<nTypeface_h> typefaces;

    SDL_Renderer* sdlRenderer;
    SDL_Window* sdlWindow;
} nDrawingContext;

/******************************************************************************
 * MARK: LOCAL FUNCTION PROTOTYPES
 *****************************************************************************/

/******************************************************************************
 * MARK: LOCAL VARIABLES
 *****************************************************************************/

static bool isInitialized = false;

/******************************************************************************
 * MARK: GLOBAL FUNCTION IMPLEMENTATIONS
 *****************************************************************************/

nDrawingContext_h NanoDraw_SetupForWindow(nWindow_h window, void *sdlWindow, void *sdlRenderer)
{

    printf("SETTING UP WINDOW\n");

    if (!isInitialized)
    {
        
       
        isInitialized = true;
    }


    nDrawingContext* context = new nDrawingContext;
    context->imguiContext = ImGui::CreateContext();
        ImGui::SetCurrentContext(context->imguiContext);

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        ImGui::StyleColorsDark();

        
    ImGui_ImplSDL2_InitForSDLRenderer((SDL_Window*)sdlWindow, (SDL_Renderer*)sdlRenderer);
    ImGui_ImplSDLRenderer2_Init((SDL_Renderer*)sdlRenderer);


    context->sdlRenderer = (SDL_Renderer*)sdlRenderer;
    context->sdlWindow = (SDL_Window*)sdlWindow;

    return context;
}

void NanoDraw_BeginFrame(nDrawingContext_h context)
{
    ImGui::SetCurrentContext(context->imguiContext);
    ImGui_ImplSDL2_NewFrame();
    ImGui_ImplSDLRenderer2_NewFrame();

    ImGui::NewFrame();
     
    context->imguiDrawList = ImGui::GetForegroundDrawList();

}

void NanoDraw_EndFrame(nDrawingContext_h context)
{
   

    ImGui::Render();
    
    SDL_SetRenderDrawColor(context->sdlRenderer, 0, 0, 0, 255);
    SDL_RenderClear(context->sdlRenderer);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), context->sdlRenderer);
    SDL_RenderPresent(context->sdlRenderer);

}

void NanoDraw_Draw(nDrawingContext_h context, nDrawCommand command, nDrawCommandData data)
{

    switch (command)
    {

        case SET_FILL_COLOR:
        {
            context->state.fillColor = data.fillColorData.color;
        } break;

        case SET_TYPEFACE:
        {

            for (nTypeface_h tf : context->typefaces)
            {
                if (tf == data.typefaceData.typeface)
                {
                    context->state.typeface = tf;
                    break;
                }
            }
          
        } break;

        case SET_ORIGIN:
        {
            context->state.x = data.originData.x;
            context->state.y = data.originData.y;
        } break;

        case SET_SIZE:
        {
            context->state.width = data.sizeData.width;
            context->state.height = data.sizeData.height;
        } break;

        case SET_TEXT:
        {
            context->state.text = data.textData.text;
        } break;

        case DRAW_TEXT:
        {   
            if (context->state.typeface != NULL)
            {
                ImGui::PushFont(context->state.typeface->imguiFont);   
            }
            
            context->imguiDrawList->AddText(
                ImVec2(context->state.x, context->state.y), 
                IM_COL32(
                    (uint32_t)(context->state.fillColor.r * 255.0f), 
                    (uint32_t)(context->state.fillColor.g * 255.0f), 
                    (uint32_t)(context->state.fillColor.b * 255.0f), 
                    (uint32_t)(context->state.fillColor.a * 255.0f)
                ), 
                context->state.text
            );

            if (context->state.typeface != NULL)
            {
                ImGui::PopFont();
            }

        } break;

        case DRAW_RECT:
        {
            SDL_SetRenderDrawColor(context->sdlRenderer, context->state.fillColor.r, context->state.fillColor.g, context->state.fillColor.b, context->state.fillColor.a);
            SDL_Rect rect;
            rect.x = context->state.x;
            rect.y = context->state.y;
            rect.w = context->state.width;
            rect.h = context->state.height;
            SDL_RenderFillRect(context->sdlRenderer, &rect);
        } break;
    }
}

nTypeface_h NanoDraw_CreateTypeface(nDrawingContext_h context, const char* typeface, float size)
{

    for (nTypeface_h tf : context->typefaces)
    {
        if (strcmp(tf->typeface, typeface) == 0 && tf->size == size)
        {
            return tf;
        }
    }
    nTypeface* typefaceVariant = new nTypeface();
    typefaceVariant->typeface = typeface;
    typefaceVariant->size = size;
    typefaceVariant->imguiFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(typeface, size);
    context->typefaces.push_back(typefaceVariant);

    return typefaceVariant;
}

/******************************************************************************
 * MARK: LOCAL FUNCTION IMPLEMENTATIONS
 *****************************************************************************/

