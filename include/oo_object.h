#ifndef OO_OBJECT_H
#define OO_OBJECT_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "amxxmodule.h"

namespace oo
{
	using Variable = std::vector<cell>;

	struct Class;

	struct Object
	{
		std::weak_ptr<Class> isa;
		std::unordered_map<std::string, Variable> vars;
	};
}

#endif // OO_OBJECT_H