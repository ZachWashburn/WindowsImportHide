
/*
MIT License
Copyright (c) 2019 Daniel Krupiński
Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#ifndef _H_FNV
#define _H_FNV
#include <cstdint>

class fnv
{
public:
    static const unsigned int FNV_PRIME = 16777619u;
    static const unsigned int OFFSET_BASIS = 2166136261u;
    template <unsigned int N>
    static constexpr unsigned int hash(const char(&str)[N], unsigned int I = N)
    {
        return I == 1 ? (OFFSET_BASIS ^ str[0]) * FNV_PRIME : (hash(str, I - 1) ^ str[I - 1]) * FNV_PRIME;
    }
    static unsigned int hashRuntime(const char* str)
    {
        const size_t length = strlen(str) + 1;
        unsigned int hash = OFFSET_BASIS;
        for (size_t i = 0; i < length; ++i)
        {
            hash ^= *str++;
            hash *= FNV_PRIME;
        }
        return hash;
    }
    struct Wrapper
    {
        Wrapper(const char* str) : str(str) { }
        const char* str;
    };
    unsigned int hash_value;
public:
    // calulate in run-time
    fnv(Wrapper wrapper) : hash_value(hashRuntime(wrapper.str)) { }
    // calulate in compile-time
    template <unsigned int N>
    constexpr fnv(const char(&str)[N]) : hash_value(fnvHashConst(str)) { }
    // output result
    constexpr operator unsigned int() const { return this->hash_value; }
};
#endif