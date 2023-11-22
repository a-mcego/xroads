#pragma once

#include <iostream>
namespace Xroads
{
    void DieWithTrace();


    //this removes the path from __FILE__ so my output doesn't get polluted
    inline consteval int WranglePath(const char* c)
    {
        int last_slash=0;
        int n=0;
        while(c[n] != 0)
        {
            if (c[n] == '/' || c[n] == '\\')
                last_slash=n;
            ++n;
        }
        return last_slash+1;
    }
#define XrAssert(x,op,y) (((x) op (y))?(void)0:(std::cerr << "Assert failed in " << (__FILE__+Xroads::WranglePath(__FILE__)) << ":" << __LINE__ << ": " << #x << #op << #y << ", with " << (x) << #op << (y) << std::endl, Kill("")))
//#define XrAssert(x,op,y) (((x) op (y))?(void)0:(std::cerr << "Assert failed in " << (__FILE__+WranglePath(__FILE__)) << ":" << __LINE__ << ": " << #x << #op << #y << ", with " << (x) << #op << (y) << std::endl << "Stacktrace: " << std::to_string(std::stacktrace::current()) << std::endl, std::abort()))
}
