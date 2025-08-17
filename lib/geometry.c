/***************************************************************
**
** NanoKit Library Source File
**
** File         :  geometry.c
** Module       :  nanodraw
** Author       :  SH
** Created      :  2025-08-17 (YYYY-MM-DD)
** License      :  MIT
** Description  :  NanoKit Geometry API
**
***************************************************************/

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include "geometry.h"

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

nkColor_t nkColor_FromHexRGB(uint32_t hex)
{
    return (nkColor_t) {
        .r = (float)((hex >> 16) & 0xFF) / 255.0f,
        .g = (float)((hex >> 8) & 0xFF) / 255.0f,
        .b = (float)(hex & 0xFF) / 255.0f,
        .a = 1.0f
    };
}

nkColor_t nkColor_FromHexARGB(uint32_t hex)
{
    return (nkColor_t) {
        .r = (float)((hex >> 16) & 0xFF) / 255.0f,
        .g = (float)((hex >> 8) & 0xFF) / 255.0f,
        .b = (float)(hex & 0xFF) / 255.0f,
        .a = (float)((hex >> 24) & 0xFF) / 255.0f
    };
}

nkColor_t nkColor_Lighten(nkColor_t color, float gain)
{
    return (nkColor_t) {
        .r = color.r + gain * (1.0f - color.r),
        .g = color.g + gain * (1.0f - color.g),
        .b = color.b + gain * (1.0f - color.b),
        .a = color.a
    };
}

nkColor_t nkColor_Darken(nkColor_t color, float gain)
{
    return (nkColor_t) {
        .r = color.r - gain * color.r,
        .g = color.g - gain * color.g,
        .b = color.b - gain * color.b,
        .a = color.a
    };
}

nkThickness_t nkThickness_FromConstant(float value)
{
    return (nkThickness_t) {
        .top = value,
        .right = value,
        .bottom = value,
        .left = value
    };
}

nkThickness_t nkThickness_FromHorizontalVertical(float horizontal, float vertical)
{
    return (nkThickness_t) {
        .top = vertical,
        .right = horizontal,
        .bottom = vertical,
        .left = horizontal
    };
}

/***************************************************************
** MARK: STATIC FUNCTIONS
***************************************************************/
