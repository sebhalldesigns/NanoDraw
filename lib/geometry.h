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

/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

#endif /* GEOMETRY_H */