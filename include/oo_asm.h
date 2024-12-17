#ifndef OO_ASM_H
#define OO_ASM_H

#include "amxxmodule.h"

cell AsmRegisterForward(AMX *amx, const char *callback, long *args_data, int start, int esp_size);
cell AsmExecuteForward(int fwd, long *arg_data, int start, int esp_size);

#endif