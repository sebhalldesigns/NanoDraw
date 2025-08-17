/***************************************************************
**
** NanoKit Library Header File
**
** File         :  geometry.h
** Module       :  nanodraw
** Author       :  SH
** Created      :  2025-02-23 (YYYY-MM-DD)
** License      :  MIT
** Description  :  Shared Geometry Types
**
***************************************************************/

#ifndef GEOMETRY_H
#define GEOMETRY_H

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include <stdint.h>

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

typedef struct 
{
    union
    {
        float x;
        float width;
        float u;
    };

    union
    {
        float y;
        float height;
        float v;
    };
} nkVector2_t;

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
        float width;
    };

    union
    {
        float a;
        float w;
        float height;
    };
} nkVector4_t;

/* aliases */
typedef nkVector2_t nkPoint_t;
typedef nkVector2_t nkSize_t;
typedef nkVector4_t nkColor_t;
typedef nkVector4_t nkRect_t;

typedef struct
{
    float top;
    float right;
    float bottom;
    float left;
} nkThickness_t;


nkColor_t nkColor_FromHexRGB(uint32_t hex);
nkColor_t nkColor_FromHexARGB(uint32_t hex);

nkThickness_t nkThickness_FromConstant(float value);
nkThickness_t nkThickness_FromHorizontalVertical(float horizontal, float vertical);

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

#endif /* GEOMETRY_H */