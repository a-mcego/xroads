#pragma once

#include "Kill.h"

namespace Xroads
{
    template<typename T>
    struct OptionRef
    {
    private:
        T* data{nullptr}; //non-owning!
    public:
        OptionRef(){}
        OptionRef(T& ref):data{&ref}{}

        T& unwrap() const
        {
            if (data == nullptr)
                Kill("OptionRef null pointer unwrap");
            return *data;
        }
    };

    template<typename T>
    struct OptionCRef
    {
    private:
        const T* data{nullptr}; //non-owning!
    public:
        OptionCRef(){}
        OptionCRef(const T& ref):data{&ref}{}

        const T& unwrap() const
        {
            if (data == nullptr)
                Kill("OptionCRef null pointer unwrap");
            return *data;
        }
    };
}
