#pragma once

#include <sstream>
#include <string>
#include <cstring>
#include "Coords.h"

//TODO: should this be moved somewhere else?
inline std::ostream& operator<<(std::ostream& o, const Xroads::C3i& v)
{
    o << "(" << v.x << "," << v.y << "," << v.z << ")";
    return o;
}

namespace Xroads
{
    struct CharPtrStr
    {
        char* data=nullptr;

        CharPtrStr()
        {
            data = new char[1];
            data[0] = '\0';
        }

        CharPtrStr& operator+=(const CharPtrStr& rhs)
        {
            size_t len = strlen(data);
            size_t rhslen = strlen(rhs.data);
            char* newdata = new char[len+rhslen+1];
            for(size_t i=0; i<len; ++i)
                newdata[i] = data[i];
            for(size_t i=0; i<rhslen; ++i)
                newdata[rhslen+i] = rhs.data[i];
            newdata[len+rhslen] = '\0';
            delete [] data;
            data = newdata;
            return *this;
        }
    };


    inline const std::vector<std::string> Explode(const std::string& s, const char& c)
    {
        std::string buff{""};
        std::vector<std::string> v;

        for(auto n:s)
        {
            if(n != c) buff+=n; else
            if(n == c && buff != "") { v.push_back(buff); buff = ""; }
        }
        if(buff != "") v.push_back(buff);

        return v;
    }

    template<typename T>
    T FromString(const std::string& key)
    {
        if constexpr(std::is_same_v<T,const std::string&>)
            return key;

        std::stringstream sstr(key);
        T ret;
        sstr >> ret;
        return ret;
    }

    template<typename T>
    std::string ToString(T key)
    {
        if constexpr(std::is_same_v<T,std::string>)
            return key;

        std::stringstream sstr;
        sstr << key;
        return sstr.str();
    }
}

