#ifndef GLOBALS_H
#define GLOBALS_H

//#pragma once // Prevent multiple declarations and definitions.

// Qt Headers.
#include <QApplication>
#include <QDateTime>
#include <QSysInfo>
#include <QString>
#include <QDir>

// Qt Networking modules.
#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QHostInfo>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>

// C++ Standard headers.
#include <math.h>
#include <random>
#include <map>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <csignal>
#include <type_traits>
#include <cctype>
#include <typeinfo>
#include <queue>

// System Sizes.
#if !defined(WORD_SZ)
#define WORD_SZ (64)
#endif //#if !defined()

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
#define VER_MINOR 2
#define VER_PATCH 0
#define VER_STAGE "ALPHA"
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

enum ECommand
{
    Cmd_NewGame,
    Cmd_PlayGame,
    Cmd_PauseGame,
    Cmd_StopGame,
    Cmd_Move,
    Cmd_Redraw,
    Cmd_Help,
    Cmd_Quit,
    Cmd_Stats,
    Cmd_ConnectToServer,
    Cmd_SetupServer,
    Cmd_Unknown
};

// Custom data structures.
struct SPoint
{
    float mX;
    float mY;

    float x() { return mX; }
    float y() { return mY; }

    void setX(float aX) { mX = aX; }
    void setY(float aY) { mY = aY; }

    SPoint() : mX{0.0f}, mY{0.0f} { /* Intentionally left blank. */ }
    SPoint(float aX, float aY) : mX{aX}, mY{aY} { /* Intentionally left blank. */ }
};

struct CfgVars
{
    bool mbIsDebug = false; //!< Are we in a debugging mode?
    std::string msProgName = "ColorWars"; //!< The name of the program.
    std::string msLogName = ""; //!< The log filename to write to.
    std::string msRootDir = "./"; //!< The root directory for the game.
};

struct SCommand
{
    ECommand meCmd;
    std::vector<std::string> mvArgs;
    std::string msSender;
    u32 muSenderID;

    SCommand(ECommand eCmd = Cmd_Redraw, std::vector<std::string> vArgs = {}, std::string sSender = "", u32 uSenderID = 0) : meCmd{eCmd}, mvArgs{vArgs}, msSender{sSender}, muSenderID{uSenderID}
    { /* Intentionally left blank. */ }
};

// External functions and variables.
extern std::map<ECellColors, QString> g_ColorNameMap;
extern std::map<std::string, ECellColors> g_NameToColorMap;
extern CfgVars g_cfgVars;

extern SCommand ParseCommandString(QString lStr, std::string sSender = "", u32 uSenderID = 0);
extern void HandleNetLogging(std::string lMsg);

#endif // GLOBALS_H
