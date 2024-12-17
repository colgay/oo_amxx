#ifndef OO_CLASS_H
#define OO_CLASS_H

#include <memory>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace oo
{
	using AmxxForward = int;
	using ArgTypeList = std::vector<int>;
	using HookList    = std::vector<int>;

	struct Constructor
	{
		AmxxForward forward;
		ArgTypeList args;
		HookList    pre;
		HookList    post;
	};

	struct Destructor
	{
		AmxxForward forward;
		HookList    pre;
		HookList    post;
	};

	struct Method
	{
		AmxxForward forward;
		bool        is_static;
		ArgTypeList args;
		HookList    pre;
		HookList    post;
	};

	struct Class : std::enable_shared_from_this<Class>
	{
		std::string name;
		std::size_t instance_size;
		
		std::vector<std::weak_ptr<Class>> supers;
		std::vector<std::weak_ptr<Class>> mro;

		std::unordered_map<std::string, int> 			vars;
		std::unordered_map<int, Constructor> 			ctors;
		std::unordered_map<std::string, Constructor *> 	ctors_map;
		std::unordered_map<std::string, Method> 		mthds;
		Destructor dtor;

		Class(const std::string &name) : name(name), instance_size(0), dtor({-1}) {}

		Class(const std::string &name, std::vector<std::weak_ptr<Class>> &&supers)
			: name(name), instance_size(0), supers(std::move(supers)), dtor({-1})
		{
			for (auto super : this->supers)
			{
				this->instance_size += super.lock()->instance_size;
			}
		}

		void InitMRO()
		{
			std::queue<std::weak_ptr<Class>> to_visit;
			std::unordered_set<std::shared_ptr<Class>> visited;
			to_visit.push(shared_from_this());

			while (!to_visit.empty())
			{
				auto current = to_visit.front();
				to_visit.pop();

				if (visited.find(current.lock()) == visited.end())
				{
					visited.emplace(current);

					for (auto super : current.lock()->supers)
					{
						to_visit.emplace(super);
					}

					this->mro.emplace_back(current);
				}
			}
		}

		void AddVariable(const std::string &name, int size)
		{
			this->instance_size += size;
			this->vars.emplace(name, size);
		}

		void AddConstructor(Constructor &&ctor, const std::string &name)
		{
			int arg_size = ctor.args.size();
			auto pair = this->ctors.emplace(arg_size, std::move(ctor));
			if (pair.second)
				this->ctors_map.emplace(name, &pair.first->second);
		}

		void AddMethod(const std::string &name, Method &&mthd)
		{
			this->mthds.emplace(name, std::move(mthd));
		}

		void SetDestructor(Destructor &&dtor)
		{
			this->dtor = std::move(dtor);
		}

		bool IsClass(std::weak_ptr<Class> cls)
		{
			return shared_from_this() == cls.lock();
		}

		bool IsSubclassOf(std::weak_ptr<Class> cls)
		{
			for (auto current : this->mro)
			{
				if (current.lock() == cls.lock())
					return true;
			}

			return false;
		}
	};
}

#endif // OO_CLASS_H