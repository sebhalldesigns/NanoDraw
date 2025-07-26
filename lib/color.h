/***************************************************************
**
** NanoKit Library Header File
**
** File         :  color.h
** Module       :  nanodraw
** Author       :  SH
** Created      :  2025-02-23 (YYYY-MM-DD)
** License      :  MIT
** Description  :  Shared Color Types
**
***************************************************************/

#ifndef COLOR_H
#define COLOR_H

/***************************************************************
** MARK: INCLUDES
***************************************************************/

#include <stdint.h>
#include "geometry.h"

/***************************************************************
** MARK: CONSTANTS & MACROS
***************************************************************/

/***************************************************************
** MARK: TYPEDEFS
***************************************************************/

#define NK_COLOR_BLACK      (nkColor_t){0.0f, 0.0f, 0.0f, 1.0f}
#define NK_COLOR_WHITE      (nkColor_t){1.0f, 1.0f, 1.0f, 1.0f}
#define NK_COLOR_RED        (nkColor_t){1.0f, 0.0f, 0.0f, 1.0f}
#define NK_COLOR_GREEN      (nkColor_t){0.0f, 1.0f, 0.0f, 1.0f}
#define NK_COLOR_BLUE       (nkColor_t){0.0f, 0.0f, 1.0f, 1.0f}
#define NK_COLOR_YELLOW     (nkColor_t){1.0f, 1.0f, 0.0f, 1.0f}
#define NK_COLOR_CYAN       (nkColor_t){0.0f, 1.0f, 1.0f, 1.0f}
#define NK_COLOR_ORANGE     (nkColor_t){1.0f, 0.5f, 0.0f, 1.0f}
#define NK_COLOR_MAGENTA    (nkColor_t){0.75f, 0.0f, 0.75f, 1.0f}
#define NK_COLOR_GRAY       (nkColor_t){0.5f, 0.5f, 0.5f, 1.0f}
#define NK_COLOR_LIGHT_GRAY (nkColor_t){0.75f, 0.75f, 0.75f, 1.0f}
#define NK_COLOR_DARK_GRAY  (nkColor_t){0.25f, 0.25f, 0.25f, 1.0f}

#define NK_COLOR_TRANSPARENT (nkColor_t){0.0f, 0.0f, 0.0f, 0.0f}



/***************************************************************
** MARK: FUNCTION DEFS
***************************************************************/

#endif /* COLOR_H */