#include "amxxmodule.h"
#include "oo_natives.h"
#include "oo_manager.h"
#include "oo_call.h"

void OnAmxxAttach(void)
{
	static const AMX_NATIVE_INFO oo_natives[] =
	{
		{ "oo_class", 	oo::native::native_class },
		{ "oo_var", 	oo::native::native_var },
		{ "oo_ctor", 	oo::native::native_ctor },
		{ "oo_dtor", 	oo::native::native_dtor },
		{ "oo_mthd", 	oo::native::native_mthd },
		{ "oo_smthd", 	oo::native::native_smthd },

		{ "oo_new", 		oo::native::native_new },
		{ "oo_delete", 		oo::native::native_delete },
		{ "oo_set", 		oo::native::native_set },
		{ "oo_set_str", 	oo::native::native_set_str },
		{ "oo_get", 		oo::native::native_get },
		{ "oo_get_str", 	oo::native::native_get_str },
		{ "oo_call", 		oo::native::native_call },
		{ "oo_super_ctor", 	oo::native::native_super_ctor },

		{ "oo_class_exists", 	oo::native::native_class_exists },
		{ "oo_object_exists", 	oo::native::native_object_exists },
		{ "oo_get_classname", 	oo::native::native_get_classname },
		{ "oo_subclass_of", 	oo::native::native_subclass_of },
		{ "oo_isa", 			oo::native::native_isa },
		{ "oo_this", 			oo::native::native_this },
		{ "oo_get_var_size", 	oo::native::native_get_var_size },
		{ "oo_get_str_len", 	oo::native::native_get_str_len },

		{ "oo_hook_ctor", 			oo::native::native_hook_ctor },
		{ "oo_hook_dtor", 			oo::native::native_hook_dtor },
		{ "oo_hook_mthd", 			oo::native::native_hook_mthd },
		{ "oo_hook_get_return", 	oo::native::native_hook_get_return },
		{ "oo_hook_set_return", 	oo::native::native_hook_set_return },
		{ "oo_hook_set_param", 		oo::native::native_hook_set_param },

		{ "oo_version", 	oo::native::native_version },
		{ "oo_print", 		oo::native::native_print },
		{ "oo_get_plugin", 	oo::native::native_get_plugin },
		
		{ nullptr, nullptr }
	};

	MF_AddNatives(oo_natives);
}

void OnPluginsLoaded()
{
	MF_ExecuteForward(
		MF_RegisterForward(
			"oo_init",
			ForwardExecType::ET_IGNORE,
			ForwardParam::FP_DONE)
	);
}

void OnPluginsUnloaded()
{
	oo::Manager::Instance()->Clear();
	oo::CallStack::Instance()->Clear();
}