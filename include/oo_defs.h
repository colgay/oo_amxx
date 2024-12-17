#ifndef OO_DEFS_H
#define OO_DEFS_H

namespace oo
{
	constexpr int OO_CONTINUE  	= 0;
	constexpr int OO_SUPERCEDE 	= 1;
	constexpr int OO_BREAK     	= 2;

	constexpr int OO_STRING_REF = -2;
	constexpr int OO_STRING     = -1;
	constexpr int OO_BYREF      = 0;
	constexpr int OO_CELL       = 1;

	constexpr int NO_FORWARD 	= -1;
	constexpr size_t OBJ_NULL 	= 0u;
}

#endif // OO_DEFS_H