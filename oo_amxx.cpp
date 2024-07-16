#include "amxxmodule.h"

void OnAmxxAttach()
{
}

void OnPluginsLoaded()
{
	MF_PrintSrvConsole("OO is loaded\n");
}

void OnPluginsUnloaded()
{
	MF_PrintSrvConsole("OO is unloaded\n");
}