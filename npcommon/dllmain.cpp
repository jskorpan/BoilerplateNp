#include <windows.h>
#include <stdio.h>

BOOL APIENTRY DllMain(HMODULE hModule,
											DWORD ul_reason_for_call,
											LPVOID lpRerved)
{

	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(hModule);
		break;
	case DLL_PROCESS_DETACH :
		break;

	default: break;
	}
	return TRUE;
}