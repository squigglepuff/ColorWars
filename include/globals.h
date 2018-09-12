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
#define TESS_COMBSZ_TO_TESSSZ (static_cast<float>(2.6542f / 1.0f))
#define TESS_NEAR_SIDE (static_cast<float>(2.308f / 1.0f))
#define TESS_NEAR_TO_FAR (static_cast<float>(1.9932f / 1.0f))
#define TESS_X_SHIFT (static_cast<float>(0.1864f / 1.0f))
#define TESS_Y_SHIFT (static_cast<float>(0.9754f / 1.0f))
#define TESS_ROTATION (10.929488f) //!< This is calculated using the values in "docs/tessellation_far_dissection.png". It's the value of tan-1(112/580).
#endif  // #if !defined(_TESS_RATIOS)

#if !defined(M_PI)
#define M_PI (3.14159265359)
#endif

#if !defined(MAX_DEGREE)
#define MAX_DEGREE 360.0f
#endif

#if !defined(VER_MAJOR)
#define VER_MAJOR 0
#define VER_MINOR 1
#define VER_PATCH 0
#define VER_STAGE "IN-DEV"
#endif //#if !defined(VER_MAJOR)

// Enumerations.
#if !defined(_CELL_COLORS)
#define _CELL_COLORS
enum ECellColors
{
    Cell_White = 0xc001,
    Cell_Red = 0xc002,
    Cell_Orange = 0xc003,
    Cell_Yellow = 0xc004,
    Cell_Lime = 0xc005,
    Cell_Green = 0xc006,
    Cell_Cyan = 0xc007,
    Cell_Blue = 0xc008,
    Cell_Purple = 0xc009,
    Cell_Magenta = 0xc00a,
    Cell_Pink = 0xc00b,
    Cell_Brown = 0xc00c,
    Cell_Gray = 0xc00d,
    Comb_Mixed = 0xcff1 //!< Only used by honeycombs and not individual cells!
};
#endif // #if !defined(_CELL_COLORS)

#endif // GLOBALS_H
