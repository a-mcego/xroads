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
