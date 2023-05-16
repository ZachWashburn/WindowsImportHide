# WindowsImportHide

This project utilizes a custom GetProcAddress implementation along side XOR'd strings to discourage static analysis and reverse engineering. 

Loadlibrary can be replace by a manual mapper implementation to further disguise functionality. 

Function types are automatically deduced, and a structure object is generated that automatically accepts parameters, all that is required is a 
underscore pre-pended to the functions name. 

All functions go through a single dispatcher function, further obscuring functionality.

## Example Usage:
```
	WINDOWS_IMPORT_HIDE(MessageBoxA, "User32.dll");
	_MessageBoxA(_GetActiveWindow(), XorStr("Hello!"), XorStr("This is a Message Box!"), MB_OK);
```
