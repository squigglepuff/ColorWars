#ifndef GLOBALS_H
#define GLOBALS_H

// C++ Standard headers.
#include <math.h>

// Custom data types.
#if !defined(_CUSTOM_TYPES_SET)
typedef unsigned char byte;
typedef byte u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
#define _CUSTOM_TYPES_SET
#endif // #if !defined(_CUSTOM_TYPES_SET)

// MACROs.
#if !defined(SZ)
#define SZ(x) (sizeof(x)/sizeof(*(x)))
#endif // #if !defined(SZ)

// Geometric constants.
#if !defined(NUM_HEX_VERTS)
#define NUM_HEX_VERTS (6)
#endif // #if !defined(NUM_HEX_VERTS)

#if !defined(_HEX_RATIOS)
#define HEX_LONG_SHORT  (static_cast<float>(1.0f / 2.0f))
#define HEX_SHORT_START  (static_cast<float>(1.0f / 4.0f))
#define HEX_HALF_WIDTH  (static_cast<float>(0.864f / 1.0f))
#define CELL_COMB_RATIO (1.71875f) //!< This is pulled from "docs/honeycomb_dissection.png". That is the value of 220/128.
#define _HEX_RATIOS
#endif // #if !defined(_HEX_RATIOS)

#if !defined(_TESS_RATIOS)
#define _TESS_RATIOS
#define TESS_COMBSZ_TO_TESSSZ (static_cast<float>(2.6864f / 1.0f))
#define TESS_NEAR_SIDE (static_cast<float>(2.308f / 1.0f))
#define TESS_NEAR_TO_FAR (static_cast<float>(1.9932f / 1.0f))
#define TESS_X_SHIFT (static_cast<float>(0.1895f / 1.0f))
#define TESS_Y_SHIFT (static_cast<float>(0.9814f / 1.0f))
#endif  // #if !defined(_TESS_RATIOS)

// Enumerations.
#if !defined(_CELL_COLORS)
#define _CELL_COLORS
enum ECellColors
{
    Cell_White,
    Cell_Red,
    Cell_Orange,
    Cell_Yellow,
    Cell_Lime,
    Cell_Green,
    Cell_Cyan,
    Cell_Blue,
    Cell_Purple,
    Cell_Magenta,
    Cell_Pink
};
#endif // #if !defined(_CELL_COLORS)

#endif // GLOBALS_H
