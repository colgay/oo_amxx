#include "oo_asm.h"

#ifdef _MSC_VER
#pragma optimize("", off)
#endif
cell AsmRegisterForward(AMX *amx, const char *callback, long *args_data, int start, int esp_size)
{
	cell result = 0;

#ifdef _MSC_VER
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
#elif __GNUC__
	asm volatile(
		"mov %1, %%ebx;"
		"mov %2, %%eax;"
		"pushl $-1;"
		"loopStart:"
		"cmp $0, %%ebx;"
		"je loopEnd;"
		"dec %%ebx;"
		"mov (%%eax,%%ebx,4),%%ecx;"
		"pushl %%ecx;"
		"jmp loopStart;"
		"loopEnd:"
		"pushl %3;"
		"pushl %4;"
		"call *%5;"
		"mov %%eax, %0;"
		"add %6, %%esp;"
		:"=r"(result):"m"(start),"m"(args_data),"r"(callback),"r"(amx),"m"(MF_RegisterSPForwardByName),"m"(esp_size):"eax","ebx","ecx");
#endif

	return result;
}

cell AsmExecuteForward(int fwd, long *args_data, int start, int esp_size)
{
	cell result = 0;

#ifdef _MSC_VER
	__asm
	{
		mov ebx, start
		mov eax, args_data
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
#elif __GNUC__
	asm volatile(
		"mov %1, %%ebx;"
		"mov %2, %%eax;"
		"loopStartE:"
		"cmp $0, %%ebx;"
		"je loopEndE;"
		"dec %%ebx;"
		"mov (%%eax,%%ebx,4),%%ecx;"
		"pushl %%ecx;"
		"jmp loopStartE;"
		"loopEndE:"
		"pushl %3;"
		"call *%4;"
		"mov %%eax, %0;"
		"add %5, %%esp;"
		:"=r"(result):"m"(start),"m"(args_data),"m"(fwd),"m"(MF_ExecuteForward),"m"(esp_size):"eax","ebx","ecx");
#endif

	return result;
}
#ifdef _MSC_VER
#pragma optimize("", on)
#endif