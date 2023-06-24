#pragma once

#include <sstream>
#include <string>

namespace Xroads
{
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

