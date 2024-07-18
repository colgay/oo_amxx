#ifndef OO_MANAGER_H
#define OO_MANAGER_H

#include <amtl/am-autoptr.h>
#include <amtl/am-vector.h>
#include <amtl/am-hashmap.h>

#include "oo_class.h"
#include "oo_object.h"

namespace oo
{
	using ObjectHash = uint32_t;
	KE_CONSTEXPR ObjectHash OBJ_NULL = 0u;

	class Manager
	{
	public:
		Manager();
		~Manager();

		Class*     NewClass(const char *name, ke::Vector<Class *> *supers=nullptr);
		ObjectHash NewObject(Class *isa);
		void       DeleteObject(ObjectHash obj_hash);

		Class*     ToClass(const char *name)      const;
		Object*    ToObject(ObjectHash obj_hash)  const;

		const Constructor* FindConstructor(Class *cls, int arg_size)   const;
		const Method*      FindMethod(Class *cls, const char *name)    const;
		Variable*          FindVariable(Object *obj, const char *name) const;

		ke::HashMap<ke::AString, ke::AutoPtr<Class>, ke::HashStringPolicy> &GetClasses() 
		{
			return m_classes;
		}

		ke::HashMap<ObjectHash, ke::AutoPtr<Object>, ke::HashIntegerPolicy> &GetObjects() 
		{
			return m_objects;
		}

		void Clear();

		static Manager* Instance();

	private:
		ke::HashMap<ke::AString, ke::AutoPtr<Class>, ke::HashStringPolicy>  m_classes;
		ke::HashMap<ObjectHash, ke::AutoPtr<Object>, ke::HashIntegerPolicy> m_objects;
	};
}

#endif // OO_MANAGER_H