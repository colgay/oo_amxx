#ifndef OO_NATIVES_H
#define OO_NATIVES_H

#include "amxxmodule.h"

namespace oo
{
	namespace native
	{
		cell AMX_NATIVE_CALL native_class(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_var(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_ctor(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_dtor(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_mthd(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_smthd(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_this(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_new(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_delete(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_set(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_get(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_call(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_super_ctor(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_version(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_print(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_version(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_class_exists(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_object_exists(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_get_classname(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_subclass_of(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_isa(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_get_var_size(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_get_str_len(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_get_str(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_set_str(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_hook_mthd(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_hook_ctor(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_hook_dtor(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_hook_get_return(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_hook_set_return(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_hook_set_param(AMX *amx, cell *params);
		cell AMX_NATIVE_CALL native_get_plugin(AMX *amx, cell *params);
	}
}

#endif // OO_NATIVES_H