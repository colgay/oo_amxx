#include "oo_manager.h"

namespace oo
{
	Manager::Manager()
	{
		m_classes.init();
		m_objects.init();
	}

	Manager::~Manager()
	{
		Clear();
	}

	Class* Manager::NewClass(const char *name, ke::Vector<Class *> *supers)
	{
		auto in = m_classes.findForAdd(name);
		m_classes.add(in, ke::AString(name), new Class(name, supers));

		return in->value;
	}

	ObjectHash Manager::NewObject(Class *isa)
	{
		Object *obj = new Object();
		obj->isa = isa;
		obj->vars.init();

		for (auto cls : isa->mro)
		{
			for (auto iter = cls->vars.iter(); !iter.empty(); iter.next())
			{
				int var_size = iter->value;

				ke::AString var_name;
				var_name.format("%s@%s", cls->name.chars(), iter->key.chars());

				auto in = obj->vars.findForAdd(var_name.chars());
				if (!in.found())
				{
					Variable var;
					var.resize(var_size);
					obj->vars.add(in, var_name, ke::Move(var));
				}
			}
		}

		auto obj_hash = ke::HashPointer(obj);
		auto in = m_objects.findForAdd(obj_hash);
		m_objects.add(in, obj_hash, ke::Move(obj));

		return obj_hash;
	}

	void Manager::DeleteObject(ObjectHash obj_hash)
	{
		auto res = m_objects.find(obj_hash);
		m_objects.remove(res);
	}

	Class* Manager::ToClass(const char *name) const
	{
		auto res = m_classes.find(name);
		if(!res.found())
			return nullptr;

		return res->value.get();
	}

	Object* Manager::ToObject(ObjectHash obj_hash) const
	{
		auto res = m_objects.find(obj_hash);
		if(!res.found())
			return nullptr;

		return res->value.get();
	}

	const Constructor* Manager::FindConstructor(Class *cls, int arg_size) const
	{
		for (auto current : cls->mro)
		{
			auto res = current->ctors.find(arg_size);
			if (res.found())
				return &res->value;
		}

		return nullptr;
	}

	const Method* Manager::FindMethod(Class *cls, const char *name) const
	{
		for (auto current : cls->mro)
		{
			auto res = current->methods.find(name);
			if (res.found())
				return &res->value;
		}

		return nullptr;
	}

	Variable* Manager::FindVariable(Object *obj, const char *name) const
	{
		if (strchr(name, '@') == NULL)
		{
			ke::AString _name;
			for (auto current : obj->isa->mro)
			{
				_name.format("%s@%s", current->name.chars(), name);

				auto res = obj->vars.find(_name.chars());
				if (res.found())
					return &res->value;
			}
		}
		else
		{
			auto res = obj->vars.find(name);
			if (res.found())
				return &res->value;
		}

		return nullptr;
	}

	void Manager::Clear()
	{
		m_classes.clear();
		m_objects.clear();
	}

	Manager *Manager::Instance()
	{
		static Manager manager;
		return &manager;
	}
}