#ifndef OO_CLASS_H
#define OO_CLASS_H

#include <amtl/am-string.h>
#include <amtl/am-vector.h>
#include <amtl/am-hashmap.h>
#include <amtl/am-hashset.h>
#include <amtl/am-deque.h>

#include "HashPolicy.h"

namespace oo
{
	using AmxxForward = int;
	using ArgTypeList = ke::Vector<int>;
	using HookList    = ke::Vector<AmxxForward>;
	static const AmxxForward kNoForward = -1;

	struct Constructor
	{
		AmxxForward forward = kNoForward;
		ArgTypeList args;
		HookList    pre;
		HookList    post;
	};

	struct Destructor
	{
		AmxxForward forward = kNoForward;
		HookList    pre;
		HookList    post;
	};

	struct Method
	{
		AmxxForward forward   = kNoForward;
		bool        is_static = false;
		ArgTypeList args;
		HookList    pre;
		HookList    post;
	};

	struct Class
	{
		ke::AString name;
		int instance_size;
		
		ke::Vector<Class *> super_classes;
		ke::Vector<Class *> mro;
		ke::HashMap<ke::AString, int, ke::HashStringPolicy>           vars;
		ke::HashMap<int, Constructor, ke::HashIntegerPolicy>          ctors;
		ke::HashMap<ke::AString, Constructor *, ke::HashStringPolicy> ctor_map;
		ke::HashMap<ke::AString, Method, ke::HashStringPolicy>        methods;
		Destructor dtor;

		Class() : Class("", nullptr) {}

		Class(const char *name, ke::Vector<Class *> *supers=nullptr)
		{
			this->instance_size = 0;

			if (supers != nullptr)
			{
				for (auto super : *supers)
				{
					this->super_classes.append(super);
					this->instance_size += super->instance_size;
				}
			}

			InitHashMap();
		}

		void InitHashMap()
		{
			this->ctors.init();
			this->vars.init();
			this->methods.init();
			this->ctor_map.init();
		}

		void InitMro()
		{
			ke::Deque<Class *> to_visit;
			ke::HashSet<Class *, ke::PointerPolicy<Class>> visited;
			visited.init();
			to_visit.append(this);

			while (!to_visit.empty())
			{
				auto current = to_visit.front();
				to_visit.popFront();

				auto r = visited.find(current);
				if (!r.found())
				{
					visited.add(current);

					for (auto super : current->super_classes)
					{
						to_visit.append(super);
					}

					this->mro.append(current);
				}
			}
		}

		void AddVariable(const char *name, int size)
		{
			this->instance_size += size;
			auto in = this->vars.findForAdd(name);
			if (!in.found())
				this->vars.add(in, ke::AString(name), size);
		}

		void AddConstructor(Constructor ctor, const char *name)
		{
			int arg_size = ctor.args.length();

			auto in = this->ctors.findForAdd(arg_size);
			if (!in.found())
				this->ctors.add(in, arg_size, ke::Move(ctor));

			auto in2 = this->ctor_map.findForAdd(name);
			if (!in2.found())
				this->ctor_map.add(in2, ke::AString(name), &in->value);
		}

		void AddMethod(const char *name, Method mthd)
		{
			auto in = this->methods.findForAdd(name);
			if (!in.found())
				this->methods.add(in, ke::AString(name), ke::Move(mthd));
		}

		void SetDestructor(Destructor dtor)
		{
			this->dtor = ke::Move(dtor);
		}

		bool IsClass(Class *cls)
		{
			return this == cls;
		}

		bool IsSubclassOf(Class *super)
		{
			for (auto cls : this->mro)
			{
				if (cls == super)
					return true;
			}

			return false;
		}
	};
}

#endif // OO_CLASS_H