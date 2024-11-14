#include "oo_manager.h"

namespace oo
{
	Manager::Manager()
	{
	}

	Manager::~Manager()
	{
		Clear();
	}

	std::weak_ptr<Class> Manager::NewClass(const std::string &name)
	{
		auto pairs = m_classes.emplace(name, std::make_shared<Class>(name));
		return pairs.first->second;
	}

	std::weak_ptr<Class> Manager::NewClass(const std::string &name, std::vector<std::weak_ptr<Class>> &&supers)
	{
		auto pairs = m_classes.emplace(name, std::make_shared<Class>(name));
		return pairs.first->second;
	}

	ObjectHash Manager::NewObject(std::weak_ptr<Class> isa)
	{
		std::shared_ptr<Object> obj = std::make_shared<Object>(isa);

		for (auto cls : isa.lock()->mro)
		{
			for (auto pairs : cls.lock()->vars)
			{
				auto name = cls.lock()->name + "@" + pairs.first;
				auto size = pairs.second;
				obj->vars.try_emplace(name, size);
			}
		}

		auto hash = std::hash<std::shared_ptr<Object>>{}(obj);
		m_objects.emplace(hash, std::move(obj));
		return hash;
	}

	void Manager::DeleteObject(ObjectHash obj_hash)
	{
		m_objects.erase(obj_hash);
	}

	std::weak_ptr<Class> Manager::ToClass(const std::string &name) const
	{
		auto iter = m_classes.find(name);
		if (iter == m_classes.end())
			return std::weak_ptr<Class>();

		return iter->second;
	}

	std::weak_ptr<Object> Manager::ToObject(ObjectHash obj_hash) const
	{
		auto iter = m_objects.find(obj_hash);
		if (iter == m_objects.end())
			return std::weak_ptr<Object>();

		return iter->second;
	}

	const Constructor* Manager::FindConstructor(std::weak_ptr<Class> cls, int arg_size) const
	{
		for (auto current : cls.lock()->mro)
		{
			auto ctors = current.lock()->ctors;
			auto iter = ctors.find(arg_size);
			if (iter != ctors.end())
				return &iter->second;
		}

		return nullptr;
	}

	const Method* Manager::FindMethod(std::weak_ptr<Class> cls, const std::string &name) const
	{
		for (auto current : cls.lock()->mro)
		{
			auto mthds = current.lock()->mthds;
			auto iter = mthds.find(name);
			if (iter != mthds.end())
				return &iter->second;
		}

		return nullptr;
	}

	const Variable* Manager::FindVariable(std::weak_ptr<Object> obj, const std::string &name) const
	{
		auto vars = obj.lock()->vars;
		std::size_t pos = name.find("@");

		if (pos != std::string::npos)
		{
			for (auto current : obj.lock()->isa.lock()->mro)
			{
				auto iter = vars.find(current.lock()->name + "@" + name);
				if (iter != vars.end())
					return &iter->second;
			}
		}
		else
		{
			auto iter = vars.find(name);
			if (iter != vars.end())
				return &iter->second;
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