#ifndef OO_OBJECT_H
#define OO_OBJECT_H

#include <amtl/am-vector.h>
#include <amtl/am-hashmap.h>

#include "HashPolicy.h"

namespace oo
{
	using Variable = ke::Vector<int32_t>;

	struct Class;

	struct Object
	{
		Class* isa;
		ke::HashMap<ke::AString, Variable, ke::HashStringPolicy> vars;
	};
}

#endif // OO_OBJECT_H