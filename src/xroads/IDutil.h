#ifndef HEADER_C31560B0487FE246
#define HEADER_C31560B0487FE246

#pragma once

namespace Xroads
{
    template<typename T>
    struct RunningID
    {
    private:
        static T current;
    public:
        static T Next()
        {
            current = current + 1;
            return current;
        }
    };

    template<typename T>
    T RunningID<T>::current = T{0};
}
#endif // header guard 

