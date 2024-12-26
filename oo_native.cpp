#include "oo_natives.h"
#include "oo_defs.h"
#include "oo_forward.h"
#include "oo_call.h"
#include "oo_manager.h"

#include <string>

extern int g_ObjectCreated;
extern int g_ObjectDeleted;

namespace oo {
namespace native
{
	cell AMX_NATIVE_CALL native_class(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);

		if (!Manager::Instance()->ToClass(_class).expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s: Duplicate class", _class);
			return 0;
		}

		auto num_params = params[0] / sizeof(cell) - 1;
		if (num_params == 0)
		{
			auto cls = Manager::Instance()->NewClass(_class);
			cls.lock()->InitMRO();
			return 1;
		}

		std::vector<std::weak_ptr<Class>> supers;
		supers.reserve(num_params);

		for (unsigned long i = 1; i <= num_params; i++)
		{
			const char *_base = MF_GetAmxString(amx, params[i+1], 1, nullptr);
			auto super = Manager::Instance()->ToClass(_base);
			if (super.expired())
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s: Base class (%s) not found", _class, _base);
				return 0;
			}

			supers.emplace_back(super);
		}

		auto cls = Manager::Instance()->NewClass(_class, std::move(supers));
		cls.lock()->InitMRO();
		return 1;
	}

	cell AMX_NATIVE_CALL native_var(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);
		int size = params[3];

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		cls.lock()->AddVariable(_name, size);
		return 1;
	}

	cell AMX_NATIVE_CALL native_ctor(AMX *amx, cell *params)
	{
		std::string _class = MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string _name = MF_GetAmxString(amx, params[2], 1, nullptr);

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.c_str(), _name.c_str());
			return 0;
		}

		std::string public_name(_class + "@" + _name);
		int num_args = params[0] / sizeof(cell) - 2;

		Constructor ctor;
		{
			if (num_args > 0)
				ctor.args.reserve(num_args);

			for (auto i = 1; i <= num_args; i++)
			{
				int size = *MF_GetAmxAddr(amx, params[i + 2]);
				ctor.args.emplace_back(size);
			}

			ctor.forward = Forward::Create(amx, public_name.c_str(), &ctor.args);

			if (ctor.forward == NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}
		
		cls.lock()->AddConstructor(std::move(ctor), _name);
		return 1;
	}

	cell AMX_NATIVE_CALL native_dtor(AMX *amx, cell *params)
	{
		std::string _class = MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string _name = MF_GetAmxString(amx, params[2], 1, nullptr);

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.c_str(), _name.c_str());
			return 0;
		}

		std::string public_name(_class + "@" + _name);

		Destructor dtor;
		{
			dtor.forward = Forward::Create(amx, public_name.c_str(), nullptr);

			if (dtor.forward == NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}

		cls.lock()->SetDestructor(std::move(dtor));
		return 1;
	}

	cell AMX_NATIVE_CALL native_mthd(AMX *amx, cell *params)
	{
		std::string _class = MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string _name = MF_GetAmxString(amx, params[2], 1, nullptr);

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.c_str(), _name.c_str());
			return 0;
		}

		std::string public_name(_class + "@" + _name);

		Method mthd;
		{
			auto num_args = params[0] / sizeof(cell) - 2;
			if (num_args > 0)
				mthd.args.reserve(num_args);

			for (auto i = 1u; i <= num_args; i++)
			{
				int size = (*MF_GetAmxAddr(amx, params[i + 2]));
				mthd.args.emplace_back(size);
			}

			mthd.forward = Forward::Create(amx, public_name.c_str(), &mthd.args);
			mthd.is_static = false;

			if (mthd.forward == NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}
		
		cls.lock()->AddMethod(_name, std::move(mthd));
		return 1;
	}

	cell AMX_NATIVE_CALL native_smthd(AMX *amx, cell *params)
	{
		std::string _class = MF_GetAmxString(amx, params[1], 0, nullptr);
		std::string _name = MF_GetAmxString(amx, params[2], 1, nullptr);

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class.c_str(), _name.c_str());
			return 0;
		}

		std::string public_name(_class + "@" + _name);

		Method mthd;
		{
			auto num_args = params[0] / sizeof(cell) - 2;
			if (num_args > 0)
				mthd.args.reserve(num_args);

			for (auto i = 1u; i <= num_args; i++)
			{
				int size = (*MF_GetAmxAddr(amx, params[i + 2]));
				mthd.args.emplace_back(size);
			}

			mthd.forward = Forward::Create(amx, public_name.c_str(), &mthd.args);
			mthd.is_static = true;

			if (mthd.forward == NO_FORWARD)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", public_name.c_str());
				return 0;
			}
		}

		cls.lock()->AddMethod(_name, std::move(mthd));
		return 1;
	}

	cell AMX_NATIVE_CALL native_this(AMX *amx, cell *params)
	{
		return CallStack::Instance()->Get()->_this;
	}

	cell AMX_NATIVE_CALL native_get(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		const char *_name = MF_GetAmxString(amx, params[2], 0, nullptr);

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Object (%d) not found", _name, hash);
			return 0;
		}

		auto var = Manager::Instance()->FindVariable(obj, _name);
		if (var == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Not found", _name);
			return 0;
		}

		auto num_args = params[0] / sizeof(cell) - 2;
		auto var_size = static_cast<int>(var->size());

		if (num_args == 0)
		{
			if (var_size == 1)
			{
				return (*var)[0];
			}
			else
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Is not a cell (size: %d), please copy by value instead", _name, var_size);
				return 0;
			}
		}
		else
		{
			if (var_size == 1 && num_args == 3)
			{
				MF_CopyAmxMemory(MF_GetAmxAddr(amx, params[3]), var->data(), 1);
				return (*var)[0];
			}
			else
			{
				if (num_args < 5)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Required at least 5 args to get array values (now: %d)", _name, num_args);
					return 0;
				}

				int from_begin = *MF_GetAmxAddr(amx, params[3]);
				if (from_begin < 0)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: The SRC begin (%d) must be >= 0", _name, from_begin);
					return 0;
				}
				int from_end = *MF_GetAmxAddr(amx, params[4]);
				if (from_begin > from_end)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: The SRC begin (%d) is bigger than the end (%d)", _name, from_begin, from_end);
					return 0;
				}
				if (from_end > var_size)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: The SRC end (%d) is bigger than actual variable size (%d)", _name, from_end, var_size);
					return 0;
				}

				int to_begin = *MF_GetAmxAddr(amx, params[6]);
				if (to_begin < 0)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: The DEST begin (%d) must be >= 0", _name, to_begin);
					return 0;
				}
				int to_end = *MF_GetAmxAddr(amx, params[7]);
				if (to_begin > to_end)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: The DEST begin (%d) is bigger than the end(%d)", _name, to_begin, to_end);
					return 0;
				}

				int from_diff = (from_end == 0) ? var_size : from_end - from_begin;
				int to_diff   = (to_end == 0) ? var_size : to_end - to_begin;
				if (from_diff != to_diff)
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: The SRC size (%d) differs from the DEST size (%d)", _name, from_diff, to_diff);
					return 0;
				}

				MF_CopyAmxMemory(MF_GetAmxAddr(amx, params[5]) + to_begin, var->data() + from_begin, to_diff);
				return (*var)[0];
			}
		}

		return 0;
	}

	cell AMX_NATIVE_CALL native_get_str(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		const char *_name = MF_GetAmxString(amx, params[2], 0, nullptr);

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Object (%d) not found", _name, hash);
			return 0;
		}

		auto var = Manager::Instance()->FindVariable(obj, _name);
		if (var == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Not found", _name);
			return 0;
		}

		auto num_args = params[0] / sizeof(cell) - 2;
		auto var_size = static_cast<int>(var->size());

		cell *arr = MF_GetAmxAddr(amx, params[3]);
		int begin = std::max(static_cast<int>(params[5]), 0);
		int slen = MF_GetAmxStringLen(var->data());
		int len = (params[4] > 0) ? std::min(slen, static_cast<int>(params[4])) : slen;
		len = std::min(len, var_size - begin);
		MF_CopyAmxMemory(arr, var->data() + begin, len+1);
		return len;
	}

	cell AMX_NATIVE_CALL native_set(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		const char* _name = MF_GetAmxString(amx, params[2], 0, nullptr);

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: Object (%d) not found", _name, hash);
			return 0;
		}

		auto var = Manager::Instance()->FindVariable(obj, _name);
		if (var == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: Not found", _name);
			return 0;
		}

		auto num_args = params[0] / sizeof(cell) - 2;
		auto var_size = static_cast<int>(var->size());

		if (var_size == 1 && num_args == 1)
		{
			(*var)[0] = *MF_GetAmxAddr(amx, params[3]);
			return 1;
		}
		else
		{
			if (num_args < 5)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: Required at least 5 args to set array values (now: %d)", _name, num_args);
				return 0;
			}

			int to_begin = *MF_GetAmxAddr(amx, params[3]);
			if (to_begin < 0)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: The DEST begin (%d) must be >= 0", _name, to_begin);
				return 0;
			}
			int to_end = *MF_GetAmxAddr(amx, params[4]);
			if (to_begin > to_end)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: The DEST begin (%d) is bigger than the end (%d)", _name, to_begin, to_end);
				return 0;
			}
			if (to_end > var_size)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: The DEST end (%d) is bigger than actual variable size (%d)", _name, to_end, var_size);
				return 0;
			}

			int from_begin = *MF_GetAmxAddr(amx, params[6]);
			if (from_begin < 0)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: The SRC begin (%d) must be >= 0", _name, from_begin);
				return 0;
			}
			int from_end = *MF_GetAmxAddr(amx, params[7]);
			if (from_begin > from_end)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: The SRC begin (%d) is bigger than the end(%d)", _name, from_begin, from_begin);
				return 0;
			}

			int from_diff = (from_end == 0) ? var_size : from_end - from_begin;
			int to_diff = (to_end == 0) ? var_size : to_end - to_begin;
			if (from_diff != to_diff)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: The SRC size (%d) differs from the DEST size (%d)", _name, from_diff, to_diff);
				return 0;
			}

			memcpy(var->data() + to_begin, MF_GetAmxAddr(amx, params[5]) + from_begin, sizeof(cell) * to_diff);
			return 1;
		}
	}

	cell AMX_NATIVE_CALL native_set_str(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		const char *_name = MF_GetAmxString(amx, params[2], 0, nullptr);

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: Object (%d) not found", _name, hash);
			return 0;
		}

		auto var = Manager::Instance()->FindVariable(obj, _name);
		if (var == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Setting Var %s: Not found", _name);
			return 0;
		}

		auto num_args = params[0] / sizeof(cell) - 2;
		auto var_size = static_cast<int>(var->size());

		cell *arr = MF_GetAmxAddr(amx, params[3]);

		int begin = std::max(static_cast<int>(params[4]), 0);
		int len = params[5];
		int slen = MF_GetAmxStringLen(arr);
		len = (len > 0) ? std::min(len, slen) : slen;
		len = std::min(len, var_size - begin);
		memcpy(var->data() + begin, arr, sizeof(cell) * (len+1));
		return len;
	}

	cell AMX_NATIVE_CALL native_get_var_size(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		const char *_name = MF_GetAmxString(amx, params[2], 0, nullptr);

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Object (%d) not found", _name, hash);
			return 0;
		}

		auto var = Manager::Instance()->FindVariable(obj, _name);
		if (var == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Not found", _name);
			return 0;
		}

		auto var_size = static_cast<int>(var->size());
		return var_size;
	}

	cell AMX_NATIVE_CALL native_get_str_len(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		const char *_name = MF_GetAmxString(amx, params[2], 0, nullptr);

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Object (%d) not found", _name, hash);
			return 0;
		}

		auto var = Manager::Instance()->FindVariable(obj, _name);
		if (var == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Getting Var %s: Not found", _name);
			return 0;
		}

		return MF_GetAmxStringLen(var->data());
	}

	cell AMX_NATIVE_CALL native_new(AMX *amx, cell *params)
	{
		std::string _class = MF_GetAmxString(amx, params[1], 0, nullptr);

		auto cls = Manager::Instance()->ToClass(_class).lock();
		if (cls == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _class.c_str());
			return OBJ_NULL;
		}

		auto hash = Manager::Instance()->NewObject(cls);
		auto num_args = params[0] / sizeof(cell) - 1;

		MF_ExecuteForward(g_ObjectCreated, _class.c_str(), num_args, hash);

		auto ctor = Manager::Instance()->FindConstructor(cls, num_args);
		if (ctor != nullptr)
		{
			Forward fwd(amx, params+1+1, &(ctor->pre), &(ctor->post), hash, &(ctor->args));
			fwd.Call(ctor->forward);
		}

		return hash;
	}

	cell AMX_NATIVE_CALL native_hook_ctor(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		auto &ctors = cls.lock()->ctors_map;
		auto iter = ctors.find(_name);
		if (iter == ctors.end())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Ctor not found", _class, _name);
			return 0;
		}

		const char *_public = MF_GetAmxString(amx, params[3], 2, nullptr);

		auto ctor = iter->second;
		std::vector<AmxxForward> *fwds;
		if (!params[4])
			fwds = &ctor->pre;
		else
			fwds = &ctor->post;

		AmxxForward fwd_id = Forward::Create(amx, _public, &ctor->args);
		if (fwd_id == NO_FORWARD)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", _public);
			return 0;
		}

		fwds->emplace_back(fwd_id);
		return 1;
	}

	cell AMX_NATIVE_CALL native_hook_mthd(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_name = MF_GetAmxString(amx, params[2], 1, nullptr);

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Class not found", _class, _name);
			return 0;
		}

		auto &mthds = cls.lock()->mthds;
		auto iter = mthds.find(_name);
		if (iter == mthds.end())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s@%s(...): Method not found", _class, _name);
			return 0;
		}

		const char *_public = MF_GetAmxString(amx, params[3], 2, nullptr);
		auto mthd = &iter->second;

		std::vector<AmxxForward> *fwds;
		if (!params[4])
			fwds = &mthd->pre;
		else
			fwds = &mthd->post;

		AmxxForward fwd_id = Forward::Create(amx, _public, &mthd->args);
		if (fwd_id == NO_FORWARD)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", _public);
			return 0;
		}

		fwds->emplace_back(fwd_id);
		return 1;
	}

	cell AMX_NATIVE_CALL native_hook_dtor(AMX *amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);

		auto cls = Manager::Instance()->ToClass(_class);
		if (cls.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Class not found", _class);
			return 0;
		}

		auto *dtor = &cls.lock()->dtor;
		if (dtor->forward == NO_FORWARD)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): No dtor in this class", _class);
			return 0;
		}

		const char *_public = MF_GetAmxString(amx, params[2], 2, nullptr);

		std::vector<AmxxForward> *fwds;
		if (!params[3])
			fwds = &dtor->pre;
		else
			fwds = &dtor->post;

		AmxxForward fwd_id = Forward::Create(amx, _public, nullptr);
		if (fwd_id == NO_FORWARD)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s(...): Public not found", _public);
			return 0;
		}

		fwds->emplace_back(fwd_id);
		return 1;
	}

	cell AMX_NATIVE_CALL native_hook_set_return(AMX *amx, cell *params)
	{
		CallStack().Instance()->Get()->result = params[1];
		return 0;
	}

	cell AMX_NATIVE_CALL native_hook_get_return(AMX *amx, cell *params)
	{
		return CallStack().Instance()->Get()->result;
	}

	cell AMX_NATIVE_CALL native_hook_set_param(AMX *amx, cell *params)
	{
		// oo_hook_set_param(1, OO_CELL, 3);
		// oo_hook_set_param(2, OO_FLOAT, 5.5);
		// oo_hook_set_param(3, OO_STRING, "test");

		int pi = params[1] - 1;

		auto call = CallStack().Instance()->Get();
		auto list = call->list;
		if (call == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Current callstack has no param.");
			return 0;
		}

		if (pi < 0 || pi >= static_cast<int>(list->size()))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Param (%d) out of range (0 ~ %d)", pi + 1, list->size());
			return 0;
		}

		int type = params[2];
		if (type != list->at(pi))
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Param (%d) type (%d) doesn't match (expected: %d)", pi + 1, type, list->at(pi));
			return 0;
		}

		long *data = &call->data->at(pi);
		switch (type)
		{
			case OO_CELL:
				*data = *MF_GetAmxAddr(amx, params[3]);
				return 1;
			
			case OO_BYREF:
				MF_LogError(amx, AMX_ERR_NATIVE, "Param (%d) type is OO_BYREF, just change the value directly in the hook.", pi + 1);
				return 0;
			
			case OO_STRING:
			{
				int len = 0;
				const char *newstr = MF_GetAmxString(amx, params[3], 0, &len);
				auto &oldstr = call->strdata->at(pi);
				if (len > static_cast<int>(oldstr.size()))
				{
					oldstr = newstr;
					*data = reinterpret_cast<long>(oldstr.data());
				}
				else
				{
					memcpy(oldstr.data(), newstr, sizeof(char) * (len + 1));
					oldstr.data()[len] = '\0';
				}
				return 1;
			}

			case OO_STRING_REF:
				MF_LogError(amx, AMX_ERR_NATIVE, "Param (%d) type is OO_STRING_REF, just change the value directly in the hook.", pi + 1);
				return 0;

			default:
				MF_LogError(amx, AMX_ERR_NATIVE, "Param (%d) type is OO_ARRAY, just change the value directly in the hook.", pi + 1);
				return 0;
		}

		MF_LogError(amx, AMX_ERR_NATIVE, "Param (%d) type is invalid.", pi + 1);
		return 0;
	}

	cell AMX_NATIVE_CALL native_delete(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", hash);
			return 0;
		}

		auto isa = obj.lock()->isa.lock();
		for (auto cls : isa->mro)
		{
			const auto &dtor = cls.lock()->dtor;
			if (dtor.forward != NO_FORWARD)
			{
				Forward fwd(amx, params+1, &(dtor.pre), &(dtor.post), hash, nullptr);
				fwd.Call(dtor.forward);
			}
			MF_ExecuteForward(g_ObjectDeleted, cls.lock()->name.c_str(), hash);
		}
		
		Manager::Instance()->DeleteObject(hash);
		return 0;
	}

	cell AMX_NATIVE_CALL native_call(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		std::string _name(MF_GetAmxString(amx, params[2], 0, nullptr));

		std::string super_name;
		size_t super_pos = _name.find("@");
		if (super_pos != std::string::npos)
			super_name = _name.substr(0, _name.find("@"));

		std::shared_ptr<Class> isa;
		if (hash != 0)
		{
			auto obj = Manager::Instance()->ToObject(hash);
			if (obj.expired())
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", hash);
				return 0;
			}

			isa = obj.lock()->isa.lock();
			if (!super_name.empty())
			{
				auto super = Manager::Instance()->ToClass(super_name);
				if (super.expired())
				{
					MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s (super): No such class (%s)", _name.c_str(), super_name.c_str());
					return 0;
				}

				if (!isa->IsSubclassOf(super))
				{
					const auto &classes = Manager::Instance()->GetClasses();
					MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s (super): %s is not a super class of %s", _name.c_str(), super_name.c_str(), isa->name.c_str());
					return 0;
				}
				
				isa = super.lock();
			}

			_name = _name.substr(super_pos + 1);
		}
		else
		{
			if (super_pos == std::string::npos)
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Call of static method %s: Class name is required", _name.c_str());
				return 0;
			}

			const char *cls_name = super_name.c_str();
			auto cls = Manager::Instance()->ToClass(cls_name);
			if (cls.expired())
			{
				MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s: No such class (%s)", _name.c_str(), cls_name);
				return 0;
			}

			_name = _name.substr(super_pos + 1);
			isa = cls.lock();
		}

		auto num_args = params[0] / sizeof(cell) - 2;
		auto mthd = Manager::Instance()->FindMethod(isa, _name);
		if (mthd == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: No such method!", isa->name.c_str(), _name.c_str());
			return 0;
		}

		if (num_args != mthd->args.size())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: #args doesn't match (expected: %d, now: %d)", isa->name.c_str(), _name.c_str(), mthd->args.size(), num_args);
			return 0;
		}

		if (hash == 0 && !mthd->is_static)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Call of %s@%s: Not a static method", isa->name.c_str(), _name.c_str());
			return 0;
		}

		Forward fwd(amx, params+1+2, &(mthd->pre), &(mthd->post), hash, &(mthd->args));
		cell ret = fwd.Call(mthd->forward);
		return ret;
	}

	cell AMX_NATIVE_CALL native_super_ctor(AMX *amx, cell *params)
	{
		ObjectHash hash = CallStack::Instance()->Get()->_this;

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "*This* object is invalid");
			return 0;
		}

		const char *super_class = MF_GetAmxString(amx, params[1], 0, nullptr);

		auto super = Manager::Instance()->ToClass(super_class);
		if (super.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", super_class);
			return 0;
		}

		auto num_args = params[0] / sizeof(cell) - 1;

		auto ctor = Manager::Instance()->FindConstructor(super, num_args);
		if (ctor == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "%s: No such constructor (#args: %d)", super_class, num_args);
			return 0;
		}

		Forward fwd(amx, params+1+1, &(ctor->pre), &(ctor->post), hash, &(ctor->args));
		fwd.Call(ctor->forward);
		return 1;
	}

	void PrintArgs(const ArgTypeList *list)
	{
		MF_PrintSrvConsole("(");

		bool first = true;
		for (auto type : *list)
		{
			if (!first)
				MF_PrintSrvConsole(", ");

			switch (type)
			{
				case OO_CELL:
					MF_PrintSrvConsole("cell");
					break;
				
				case OO_BYREF:
					MF_PrintSrvConsole("byref");
					break;

				case OO_STRING:
					MF_PrintSrvConsole("str");
					break;

				case OO_STRING_REF:
					MF_PrintSrvConsole("stref");
					break;

				default:
					MF_PrintSrvConsole("arr[%d]", type);
					break;
			}

			first = false;
		}

		MF_PrintSrvConsole(")\n");
	}

	cell AMX_NATIVE_CALL native_version(AMX *amx, cell *params)
	{
		MF_SetAmxString(amx, params[1], MODULE_VERSION, params[2]);
		return 0;
	}

	cell AMX_NATIVE_CALL native_print(AMX *amx, cell *params)
	{
		MF_PrintSrvConsole("---------- [OO Classes] ----------\n");
		
		bool first = true;
		for (const auto &clpair : Manager::Instance()->GetClasses())
		{
			if (!first)
				MF_PrintSrvConsole("---\n");

			auto cl = clpair.second;
			MF_PrintSrvConsole("%s (#%p)", cl->name.c_str(), cl.get());
		
			bool first2 = true;
			for (auto su : cl->supers)
			{
				if (first2)
					MF_PrintSrvConsole(" -> ");
				else
					MF_PrintSrvConsole(", ");

				auto sc = su.lock();
				MF_PrintSrvConsole("%s (#%p)", sc->name.c_str(), sc.get());
				first2 = false;
			}

			MF_PrintSrvConsole("\n");

			if (!cl->ctors.empty())
			{
				MF_PrintSrvConsole("  Constructors:\n");

				for (const auto &pair : cl->ctors)
				{
					const auto &ct = pair.second;
					for (const auto &cpair : cl->ctors_map)
					{
						if (&ct == cpair.second)
						{
							MF_PrintSrvConsole("    %s", cpair.first.c_str());
							PrintArgs(&ct.args);
							break;
						}
					}
				}
			}

			if (!cl->mthds.empty())
			{
				MF_PrintSrvConsole("  Methods:\n");

				for (const auto &pair : cl->mthds)
				{
					const auto &mt = pair.second;
					MF_PrintSrvConsole("    %s", pair.first.c_str());
					PrintArgs(&mt.args);
				}
			}

			if (cl->dtor.forward != NO_FORWARD)
			{
				MF_PrintSrvConsole("  Destructor: yes\n");
			}

			first = false;
		}

		MF_PrintSrvConsole("----------------------------------\n");
		return 0;
	}

	cell AMX_NATIVE_CALL native_class_exists(AMX* amx, cell *params)
	{
		const char *_class = MF_GetAmxString(amx, params[1], 0, nullptr);

		if (Manager::Instance()->ToClass(_class).expired())
			return 0;
		
		return 1;
	}

	cell AMX_NATIVE_CALL native_object_exists(AMX* amx, cell *params)
	{
		ObjectHash hash = params[1];

		if (hash == OBJ_NULL || Manager::Instance()->ToObject(hash).expired())
			return 0;	// no success

		return 1;
	}

	cell AMX_NATIVE_CALL native_get_classname(AMX* amx, cell *params)
	{
		ObjectHash hash = params[1];

		auto obj = Manager::Instance()->ToObject(hash);
		if (obj.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", hash);
			return 0;	// error
		}

		const char *classname = obj.lock()->isa.lock()->name.c_str();
		if (classname[0])
		{
			MF_SetAmxString(amx, params[2], classname, params[3]);
		}

		return 1;
	}

	cell AMX_NATIVE_CALL native_subclass_of(AMX *amx, cell *params)
	{
		const char *_sub = MF_GetAmxString(amx, params[1], 0, nullptr);
		const char *_super = MF_GetAmxString(amx, params[2], 1, nullptr);
		
		auto sub = Manager::Instance()->ToClass(_sub);
		if (sub.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Sub Class (%s) not found", _sub);
			return 0;
		}

		auto super = Manager::Instance()->ToClass(_super);
		if (super.expired())
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Super Class (%s) not found", _super);
			return 0;
		}

		return sub.lock()->IsSubclassOf(super.lock());
	}

	cell AMX_NATIVE_CALL native_isa(AMX *amx, cell *params)
	{
		ObjectHash hash = params[1];
		const char *_class = MF_GetAmxString(amx, params[2], 0, nullptr);

		auto obj = Manager::Instance()->ToObject(hash).lock();
		if (obj == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Object (%d) not found", hash);
			return 0;
		}

		auto cls = Manager::Instance()->ToClass(_class).lock();
		if (cls == nullptr)
		{
			MF_LogError(amx, AMX_ERR_NATIVE, "Class (%s) not found", _class);
			return 0;
		}

		auto isa = obj->isa.lock();
		if (params[3])
		{
			if (isa->IsClass(cls))
				return 1;

			return isa->IsSubclassOf(cls) ? 1 : 0;
		}

		return isa->IsClass(cls) ? 1 : 0;
	}

	cell AMX_NATIVE_CALL native_get_plugin(AMX *amx, cell *params)
	{
		return MF_FindScriptByAmx(CallStack::Instance()->Get()->amx);
	}
}}