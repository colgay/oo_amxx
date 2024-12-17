#ifndef OO_MANAGER_H
#define OO_MANAGER_H

#include <memory>
#include <vector>
#include <unordered_map>

#include "oo_class.h"
#include "oo_object.h"

namespace oo
{
	using ObjectHash = size_t;

	class Manager
	{
	public:
		~Manager();

		std::weak_ptr<Class> NewClass(const std::string &name);
		std::weak_ptr<Class> NewClass(const std::string &name, std::vector<std::weak_ptr<Class>> &&supers);
		ObjectHash NewObject(std::weak_ptr<Class> isa);
		void       DeleteObject(ObjectHash obj_hash);

		std::weak_ptr<Class> 	ToClass(const std::string &name) const;
		std::weak_ptr<Object> 	ToObject(ObjectHash obj_hash) 	 const;

		const Constructor* 	FindConstructor(std::weak_ptr<Class> cls, int arg_size) 		 const;
		const Method* 		FindMethod(std::weak_ptr<Class> cls, const std::string &name) 	 const;
		Variable* 			FindVariable(std::weak_ptr<Object> obj, const std::string &name);

		const std::unordered_map<std::string, std::shared_ptr<Class>> &GetClasses() const
		{
			return m_classes;
		}

		const std::unordered_map<ObjectHash, std::shared_ptr<Object>> &GetObjects() const
		{
			return m_objects;
		}

		void Clear();

		static Manager* Instance();

		Manager() {}
	private:

		std::unordered_map<std::string, std::shared_ptr<Class>> m_classes;
		std::unordered_map<ObjectHash, std::shared_ptr<Object>> m_objects;
	};
}

#endif // OO_MANAGER_H