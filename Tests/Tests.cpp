// Tests.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "../WindowsImportHide/WindowsImportHide.h"
#include "../WindowsImportHide/RebuiltWindowsAPI.h"
#include "../WindowsImportHide/fnv.h"

//GLOBAL_WINDOWS_IMPORT_HIDE(LoadLibraryA, "kernel32.dll");




/// <c>LoadLibraryA</c> 
/// <summary>/// Loads the specified  module into the address space of the calling process. The specified 
/// module may cause other modules to be loaded. 
///(libloaderapi.h) </summary>
///<param name="lpLibFileName"> 
/// The name of the module. This can be either a library module (a .dll file) or an executable 
/// module (an .exe file). The name specified is the file name of the module and is not related to the 
/// name stored in the library module itself, as specified by the LIBRARY keyword in 
/// the module-definition (.def) file.If the string specifies a full path, the function searches only that path for the module.If the string specifies a relative path or a module name without a path, the function uses a standard search 
/// strategy to find the module; for more information, see the Remarks.If the function cannot find the  module, the function fails. When specifying a path, be sure to use 
/// backslashes (\), not forward slashes (/). For more information about paths, see 
/// Naming a File or Directory.If the string specifies a module name without a path and the file name extension is omitted, the function 
/// appends the default library extension ".DLL" to the module name. To prevent the function from appending 
/// ".DLL" to the module name, include a trailing point character (.) in the module name string. 
/// </param>
/// <returns>HMODULE</returns>
#if 1
GLOBAL_WINDOWS_IMPORT_HIDE(LoadLibraryA, "Kernel32.dll");
typedef HMODULE(*fnLoadLibraryA_t)(LPCSTR lpLibFileName);
GLOBAL_WINDOWS_IMPORT_HIDE(GetProcAddress, "Kernel32.dll");

#endif


NTSTATUS NTAPI LdrUnloadDll(IN HANDLE ModuleHandle);
NTSTATUS NTAPI LdrGetDllHandle(IN HANDLE ModuleHandle);
NTSTATUS NTAPI LdrGetProcedureAddres(IN HANDLE ModuleHandle);
int main()
{
	WINDOWS_IMPORT_HIDE(IsDebuggerPresent, "Kernel32.dll");

	if(_IsDebuggerPresent())
		_asm int 20

	WINDOWS_IMPORT_HIDE(MessageBoxA, "User32.dll");
	WINDOWS_IMPORT_HIDE(GetActiveWindow, "User32.dll");

	printf("start up!\n");
	HMODULE hRet = g_LoadLibraryA(XorStr("kernel32.dll"));
	HMODULE ntdll = RebuiltWindowsAPI::GetModuleA(XorStr("ntdll.dll"));
	FARPROC LdrLoadDll = g_GetProcAddress(ntdll, XorStr("LdrLoadDll"));

	WINDOWS_IMPORT_HIDE(LdrUnloadDll, "ntdll.dll");
	WINDOWS_IMPORT_HIDE(LdrGetDllHandle, "ntdll.dll");
	


	printf(XorStr("What are these imports? : %p, %p, %p \n"), LdrLoadDll, _LdrUnloadDll.m_pfnFunc, _LdrGetDllHandle.m_pfnFunc);

	_MessageBoxA(_GetActiveWindow(), XorStr("How Does This Work"), XorStr("You'll Never Figure It Out"), MB_OK);

	Sleep(99999);
}

