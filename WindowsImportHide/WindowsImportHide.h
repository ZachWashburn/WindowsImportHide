/*

Copyright (C) 2022 by Zachary Washburn

Permission is hereby granted, free of charge, to any person obtaining a copy of this
software and associated documentation files (the "Software"), to deal in the Software without restriction,
including without l> imitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial
portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

// end license

========================================================================================================
                                         Windows Import Hide
========================================================================================================

                                              OVERVIEW:

    A Method of hiding windows imports within code bases. Function names may be passed in using 
fnv hashes, allowing for no string (xor'd or plain) to appear within a compiled dll. C++20 or later
is needed due to use of constexpr if and other template functionality. Function may be called through
a ret, by defining FORCE_RET_CALLS. Compiled using c++ lastest as of 6/26/2022.


========================================================================================================
                                         Windows Import Hide
========================================================================================================
*/

#ifndef _H_WINDOWSIMPORTHIDE
#define _H_WINDOWSIMPORTHIDE

#ifndef MSC_VER
#pragma once
#endif

#define WINDOWS_IMPORT_HASH_PROC_NAMES
#define FORCE_RET_CALLS
#include "XOR_STR.h"
#include "fnv.h"
#include <type_traits>
namespace WindowsImportHide
{
    constexpr auto time = __TIME__;
    constexpr auto seed = static_cast<int>(time[7]) + static_cast<int>(time[6]) * 10 + static_cast<int>(time[4]) * 60 + static_cast<int>(time[3]) * 600 + static_cast<int>(time[1]) * 3600 + static_cast<int>(time[0]) * 36000;

	// left here until I move it out
#pragma optimize("", off)




    inline __declspec(noinline)  __declspec(naked) void* __cdecl _call_function_x86_internal()
    {
        // + 4 -> ret ebp save
        // + 8 -> o_ecx
        // + 12 -> o_edx
        // + 16 -> pstack
        // + 20 -> nstacksize
        // + 24 -> ret_storage
        // + 28 -> pop_stack
        // + 32 -> function address  
        _asm {
            push ebp
            mov ebp, esp
            mov ecx, esp
            add ecx, 20 // get nstack
            mov ecx, [ecx]
            // modify function return info
            mov esi, esp
            add esi, 16 // get pstack
            mov esi, [esi]
            mov edi, esp
            sub edi, ecx
            // https://faydoc.tripod.com/cpu/movsb.htm
            cld // set df flag so movsb increments pointers (DF = 0)
            rep movsb // copy data 
            // okay our stack is set up
            // mov registers over, go to area, and call!
            mov ebx, esp
            add ebx, 32
            mov ebx, [ebx] // function address
            mov ecx, esp
            add ecx, 8
            mov ecx, [ecx]
            mov edx, esp
            add edx, 12
            mov edx, [edx]
            // go to bottom of copied stack
            mov edi, esp
            add edi, 20
            mov edi, [edi]

            // now lets call the function
            sub esp, edi
#if 1   // through a ret to be cool lol
            mov eax, function_return
            push eax // ret address
            push ebx // function address
            ret // boom 
#else   // we are boring and call it normally...
            call ebx
#endif

        function_return:
            mov esp, ebp
                pop ebp
                ret
        }
    }

    /// <c>call_function_x86</c> 
    /// <summary> 
    /// Calls A x86 Function through a ret procedure call.
    /// Wrapper Functions are guaranteed not to touch xmm0+ float registers.
    /// Function calls requiring operands aquired in/out of the function should manually
    /// be placed on the stack before call of call_function_x86. Functional templates
    /// will automatically manage this behaviour. 
    /// NOTE : Structure return values are not 
    /// guaranteed to work. depending on compilation, register may be trashed from
    /// wrapper functions. 
    ///  </summary>
    /// <param name="pfunction"> Address of function to call </param>
    /// <param name="completed"> Returns whether the function was completed sucessfully </param>
    /// <param name="o_ecx"> value to place in the ecx register </param>
    /// <param name="o_edx"> Value to place in the edx register </param>
    /// <param name="pstack"> Pointer to the stack in which to call the function with </param>
    /// <param name="nstacksize"> Size of the stack (parameter size) </param>
    /// <param name="ret"> Holds the return value of the function </param>
    /// <returns> void (no return) </returns>
    _Ret_maybenull_ inline __declspec(noinline) void* __cdecl  call_function_x86(
        _In_ void* pfunction,
        _In_ bool& completed,
        _In_ void* o_ecx,
        _In_ void* o_edx,
        _In_reads_(nstacksize) void* pstack,
        _In_ int   nstacksize,
        _In_ unsigned long& ret)
    {
        completed = false;

        if (nstacksize > UINT8_MAX)
            return nullptr; // 1 byte add opcode

        // + 8 -> o_ecx
        // + 12 -> o_edx
        // + 16 -> pstack
        // + 20 -> nstacksize
        // + 24 -> ret_storage
        // + 28 -> pop_stack
        // + 32 -> function address

        unsigned long _ret = 0;
        _asm {
            push pfunction
            push 0
            push ret
            push nstacksize
            push pstack
            push o_edx
            push o_ecx
            call _call_function_x86_internal
            add esp, 28
            mov _ret, eax
        }
        ret = _ret;
        completed = true;
        return (void*)ret;
    }

    template <typename... Args>
    struct variadic_typedef
    {
        // this single type represents a collection of types,
        // as the template arguments it took to define it
    };

    template <typename... Args>
    struct convert_in_tuple
    {
        // base case, nothing special,
        // just use the arguments directly
        // however they need to be used
        typedef std::tuple<Args...> type;
    };

    template <typename... Args>
    struct convert_in_tuple<variadic_typedef<Args...>>
    {
        // expand the variadic_typedef back into
        // its arguments, via specialization
        // (doesn't rely on functionality to be provided
        // by the variadic_typedef struct itself, generic)
        typedef typename convert_in_tuple<Args...>::type type;
    };

    template <typename ... Ts>
    constexpr std::size_t sum_size(std::tuple<Ts...> const&)
    {
        return (sizeof(Ts) + ...);
    }


    template<size_t I = 0, typename... Tp>
    void copy_tuple_arguments(std::tuple<Tp...>& t, int& nCurPos, void* pMemory) {
        auto val = std::get<I>(t);
        memcpy((char*)pMemory + nCurPos, &val, sizeof(val));
        nCurPos += sizeof(val);
        // do things
        if constexpr (I + 1 != sizeof...(Tp))
            copy_tuple_arguments<I + 1>(t, nCurPos, pMemory);
    }

    template<class function_params_t, typename ...A>
    void* get_arguements_from_tuple(void* pMem, int& nSize, A... args)
    {
        function_params_t funcargs = { args... };
        
        if constexpr (sizeof...(A) == 0) {
            nSize = 0;
        }
        else {
            nSize = sum_size(funcargs); 

            if (!pMem)
                pMem = malloc(nSize);

            int nCurPos = 0;
            copy_tuple_arguments(funcargs, nCurPos, pMem);
        }
        return pMem;
    }

    template<class T, class function_params_t, typename ...A>
    T call_from_arg_pack(void* pfnFunc, A... args)
    {
        function_params_t funcargs = { args... };
        int nStackSize = 0;
        void* pMem = nullptr;
        void* pStack = nullptr;
        if constexpr (sizeof...(A) != 0) {
            nStackSize = sum_size<function_params_t>(funcargs);
            pMem = _malloca(nStackSize);
            pStack = get_arguements_from_tuple<function_params_t, A...>(pMem, nStackSize, args...);
        }

        bool disregard = false;
        unsigned long ret;
        call_function_x86(pfnFunc, disregard, 0, 0, pStack, nStackSize, ret);
        _freea(pMem);
        if(!std::is_same<void, T>::value)
            return (T)ret;
    }



	void* LoadWindowsFunction(const char* szModule, const char* szFuncName);
	template<class T>
	T LoadWindowsFunction(const char* szModule, const char* szFuncName) { return reinterpret_cast<T>(LoadWindowsFunction(szModule, szFuncName)); }


	void* LoadWindowsFunction(const char* szModule, unsigned long ulFuncHash);
	template<class T>
	T LoadWindowsFunction(const char* szModule, unsigned long ulFuncHash) { return reinterpret_cast<T>(LoadWindowsFunction(szModule, ulFuncHash)); }

	template<class T>
	struct _windows_import_function_t;

#ifdef FORCE_RET_CALLS
#define CALLOP() return ret_call(args...);
#else
#define CALLOP() return call(args...);
#endif
#define MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE(CallConv)												\
	template<typename T, typename ...A>																	\
	struct _windows_import_function_t<T(CallConv* )(A...)>												\
	{																									\
		using type = T(CallConv*)(A...);																\
        typedef variadic_typedef<A...> myTypes;                                                         \
        typedef convert_in_tuple<myTypes>::type function_params_t;                                      \
		_windows_import_function_t() {}																	\
		_windows_import_function_t(void* pfnFunction) : m_pfnFunc(pfnFunction) {}						\
		_windows_import_function_t(const char* szFuncName, const char* szModule)						\
		{																								\
			m_pfnFunc = (type)(seed ^ (unsigned long)LoadWindowsFunction<type>(szModule, szFuncName));								\
		}																								\
		_windows_import_function_t(unsigned long ulHash, const char* szModule)							\
		{																								\
			m_pfnFunc = (type)(seed ^ (unsigned long)LoadWindowsFunction<type>(szModule, ulHash));									\
		}																								\
        __forceinline T CallConv ret_call(A... args){return call_from_arg_pack<T, function_params_t, A...>((type)(seed ^ (unsigned long)m_pfnFunc), args...);}\
		__forceinline T CallConv call(A... args){return ((type)(seed ^ (unsigned long)m_pfnFunc))(args...);}\
		__forceinline T operator()(A... args) {CALLOP();}									            \
		void* m_pfnFunc = nullptr;																		\
	};

    // Abuse Partial Template Specilization to aquire call type of a function 
    // __forceinline T CallConv call(A... args)	{return ((T(CallConv*)(A...))m_pfnFunc)(args...);}	\
	MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE(__cdecl);
	MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE(__stdcall);
	MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE(__vectorcall);

#undef FORCE_RET_CALLS // __thiscall and __fastcall don't play nicely with return calls currently
                       // further template specialization is needed 
#define CALLOP() return call(args...);
	MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE(__thiscall);
	MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE(__fastcall);

#if defined _M_CEE_PURE || defined MRTDLL
	MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE(__clrcall);
#endif


#undef MAKE_IMPORT_FUNCTION_SPECIALIZED_TEMPLATE
#undef CALLOP

#if !defined(_DEBUG) && defined(WINDOWS_IMPORT_HASH_PROC_NAMES)
#define WINDOWS_IMPORT_HIDE(FUNCTION, MODULE) constexpr unsigned long _##FUNCTION##_hash = fnv::hash(#FUNCTION);		\
	WindowsImportHide::_windows_import_function_t<decltype(&FUNCTION)> _##FUNCTION( _##FUNCTION##_hash, XorStr(MODULE))

#define GLOBAL_WINDOWS_IMPORT_HIDE(FUNCTION, MODULE) inline constexpr unsigned long g_##FUNCTION##_hash = fnv::hash(#FUNCTION);		\
	inline WindowsImportHide::_windows_import_function_t<decltype(&FUNCTION)> g_##FUNCTION( g_##FUNCTION##_hash, XorStr(MODULE))

#define WINDOWS_IMPORT_HIDE_NO_HASH(FUNCTION, MODULE) WindowsImportHide::_windows_import_function_t<decltype(&FUNCTION)> _##FUNCTION(XorStr(#FUNCTION), XorStr(MODULE))
#define GLOBAL_WINDOWS_IMPORT_HIDE_NO_HASH(FUNCTION, MODULE) WindowsImportHide::_windows_import_function_t<decltype(&FUNCTION)> g_##FUNCTION(XorStr(#FUNCTION), XorStr(MODULE))
#else
#define WINDOWS_IMPORT_HIDE(FUNCTION, MODULE) WindowsImportHide::_windows_import_function_t<decltype(&FUNCTION)> _##FUNCTION(XorStr(#FUNCTION), XorStr(MODULE))
#define GLOBAL_WINDOWS_IMPORT_HIDE(FUNCTION, MODULE) inline WINDOWS_IMPORT_HIDE(FUNCTION, MODULE)

#define WINDOWS_IMPORT_HIDE_NO_HASH(FUNCTION, MODULE) WINDOWS_IMPORT_HIDE(FUNCTION, MODULE);
#define GLOBAL_WINDOWS_IMPORT_HIDE_NO_HASH(FUNCTION, MODULE) GLOBAL_WINDOWS_IMPORT_HIDE(FUNCTION, MODULE);
#endif

}
#endif