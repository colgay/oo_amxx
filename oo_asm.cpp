#include "oo_asm.h"

#pragma optimize("", off)
cell AsmRegisterForward(AMX *amx, const char *callback, long *args_data, int start, int esp_size)
{
	cell result = 0;
	__asm
	{
		push FP_DONE
		mov ebx, start
		mov eax, args_data
	loop_start_reg:
		cmp ebx, 0
		je loop_end_reg
		dec ebx
		mov ecx, [eax + ebx * 4]
		push ecx
		jmp loop_start_reg
	loop_end_reg:
		push callback
		push amx
		call MF_RegisterSPForwardByName
		mov result, eax
		add esp, esp_size
	}
	return result;
}

cell AsmExecuteForward(int fwd, long *arg_data, int start, int esp_size)
{
	cell result = 0;
	__asm
	{
		mov ebx, start
		mov eax, arg_data
	loop_start_exec:
		cmp ebx, 0
		je loop_end_exec
		dec ebx
		mov ecx, [eax + ebx * 4]
		push ecx
		jmp loop_start_exec
	loop_end_exec:
		push fwd
		call MF_ExecuteForward
		mov result, eax
		add esp, esp_size
	}
	return result;
}
#pragma optimize("", on)