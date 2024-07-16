#ifndef OO_DEFS_H
#define OO_DEFS_H

#include <amtl/am-cxx.h>

namespace oo
{
    KE_CONSTEXPR int OO_CONTINUE  = 0;
    KE_CONSTEXPR int OO_SUPERCEDE = 1;
    KE_CONSTEXPR int OO_BREAK     = 2;

    KE_CONSTEXPR int OO_STRING_REF = -2;
    KE_CONSTEXPR int OO_STRING     = -1;
    KE_CONSTEXPR int OO_BYREF      = 0;
    KE_CONSTEXPR int OO_CELL       = 1;
}

#endif // OO_DEFS_H