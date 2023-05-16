// WindowsImportHide.cpp : Defines the functions for the static library.
//
#include "WindowsImportHide.h"
#include "RebuiltWindowsAPI.h"
#include <Windows.h>
#include <libloaderapi2.h>
namespace WindowsImportHide
{
	void* LoadWindowsFunction(const char* szModule, const char* szFuncName)
	{
		HMODULE handle = RebuiltWindowsAPI::GetModuleA(szModule);
		if (!handle)
		{
			WINDOWS_IMPORT_HIDE(LoadLibraryA, "kernel32.dll");
			handle = _LoadLibraryA(szModule);
		}

#ifdef _DEBUG
		printf("No Func Found For %s in %s\n", szFuncName, szModule);
#endif

		void* pFunc = RebuiltWindowsAPI::GetExportAddress(handle, szFuncName);
		return 	pFunc;
	}

	void* LoadWindowsFunction(const char* szModule, unsigned long ulFuncHash)
	{
		HMODULE handle = RebuiltWindowsAPI::GetModuleA(szModule);

		if (!handle)
		{
			WINDOWS_IMPORT_HIDE(LoadLibraryA, "kernel32.dll");
			handle = _LoadLibraryA(szModule);
		}

		void* pFunc = RebuiltWindowsAPI::GetExportAddressByHash(handle, ulFuncHash);
#ifdef _DEBUG
		printf("No Hash Found For %ul in %s\n", ulFuncHash, szModule);
#endif
		return 	pFunc;
	}
}


