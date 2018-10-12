#ifndef CW_HELPERS_H
#define CW_HELPERS_H

//#pragma once

#include "include/globals.h"

#if defined(_EXTERN_CMDPARSE_FXN)
extern SCommand ParseCommandString(QString lStr); // THIS IS NOT DEFINED IN THE IMPLEMENTATION FILE! IT'S EXTERNAL!
#else
extern SCommand ParseCommandString(QString);
#endif // #if defined(_EXTERN_CMDPARSE_FXN)

extern QByteArray* PackBoardMap(std::map<u64, ECellColors> mData);
extern std::map<u64, ECellColors> UnpackBoardMap(QByteArray* pData);

#endif // CW_HELPERS_H
