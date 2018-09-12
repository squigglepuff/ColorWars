#include "include/game.h"

// ================================ Begin CDice Implementation ================================ //
CDice::CDice()
{
    // Seed the RNG.
    srand(static_cast<u32>(clock()));
}

CDice::CDice(const CDice&)
{
    // Intentionally left blank.
}

CDice::~CDice()
{
    // Intentionally left blank.
}

CDice& CDice::operator=(const CDice&)
{
    return *this;
}

u32 CDice::Roll(u32 uMin, u32 uMax)
{
    return (uMax > 0) ? (static_cast<u32>(rand()) % uMax) + uMin : static_cast<u32>(rand());
}
// ================================ End CDice Implementation ================================ //


// ================================ Begin CGame Implementation ================================ //
// ================================ End CGame Implementation ================================ //
