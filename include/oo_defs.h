#ifndef OO_DEFS_H
#define OO_DEFS_H

#include <amtl/am-cxx.h>

namespace oo
{
	static const int OO_CONTINUE  = 0;
	static const int OO_SUPERCEDE = 1;
	static const int OO_BREAK     = 2;

	static const int OO_STRING_REF = -2;
	static const int OO_STRING     = -1;
	static const int OO_BYREF      = 0;
	static const int OO_CELL       = 1;
}

#endif // OO_DEFS_H