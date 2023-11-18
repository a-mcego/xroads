#pragma once

#include <sstream>
#include <string>
#include <functional>
#include <string_view>
#include <cstring>
#include <format>
#include "Coords.h"


namespace Xroads
{
    template<size_t SIZE>
    struct SmallString
    {
        using value_type = char;
        static constexpr int MAX_SIZE = SIZE-1;

        value_type data_internal[SIZE-1] = {};
        u8 len{};

        constexpr SmallString() = default;
        constexpr SmallString(const SmallString&) = default;
        explicit constexpr SmallString(std::string_view view)
        {
            operator+=(view);
        }
        template<typename RHS>
        constexpr SmallString& operator=(const RHS& rhs) requires IsOneOf<RHS,std::string_view,SmallString>
        {
            len = 0;
            operator+=(rhs);
            return *this;
        }
        constexpr ~SmallString() = default;

        template<typename RHS>
        constexpr SmallString operator+(const RHS& rhs) requires IsOneOf<RHS,std::string_view,SmallString>
        {
            SmallString ss=*this;
            ss += rhs;
            return ss;
        }

        template<typename RHS>
        constexpr SmallString& operator+=(const RHS& rhs) requires IsOneOf<RHS,std::string_view,SmallString>
        {
            for(auto ch: std::string_view(rhs))
            {
                if (len >= MAX_SIZE)
                    break;
                data_internal[len] = ch;
                ++len;
            }
            return *this;
        }

        constexpr operator std::string_view() const
        {
            return {&data_internal[0], len};
        }

        constexpr std::string_view view() const
        {
            return std::string_view(*this);
        }

        std::string to_string() const
        {
            return std::string(begin(), end());
        }

        constexpr std::strong_ordering operator<=>(std::string_view rhs) const
        {
            return std::string_view(*this) <=> rhs;
        }
        constexpr std::strong_ordering operator<=>(const SmallString& rhs) const
        {
            return std::string_view(*this) <=> std::string_view(rhs);
        }
        constexpr bool operator==(const SmallString& rhs) const
        {
            return std::string_view(*this) == std::string_view(rhs);
        };
        constexpr size_t length() const { return len; }
        constexpr size_t size() const { return len; }

        constexpr value_type* begin() { return data_internal; }
        constexpr value_type* end() { return data_internal+size(); }

        constexpr const value_type* begin() const { return data_internal; }
        constexpr const value_type* end() const { return data_internal+size(); }

        constexpr value_type* data() { return data_internal; }
        constexpr value_type* push_back(value_type value)
        {
            if (len >= MAX_SIZE)
                return data_internal+len;
            data_internal[len] = value;
            ++len;
            return data_internal+len;
        }
    };

    using Sm16 = SmallString<16>;
    using Sm32 = SmallString<32>;

    template<size_t SIZE, typename STRINGVIEW>
    inline SmallString<SIZE> operator+(const STRINGVIEW& lhs, const SmallString<SIZE>& rhs)
    {
        SmallString<SIZE> ret(lhs);
        ret += rhs;
        return ret;
    }

    template<typename... Args>
    SmallString<32> Format(std::string_view format_string, Args&&... args)
    {
        SmallString<32> ret;
        vformat_to(std::back_inserter(ret), format_string, std::make_format_args(args...));
        return ret;
    }

    inline const std::vector<std::string_view> Explode(const std::string& s, const char& c)
    {
        std::vector<std::string_view> v;
        std::string_view sv = s;

        size_t start=0, len=0;//, i=0;
        for(auto n:s)
        {
            if(n != c) ++len; else
            if(n == c && len > 0)
            {
                ++len;
                v.push_back(sv.substr(start, len));
                start += len;
                len = -1;
            }
            //++i;
        }
        if(len > 0) v.push_back(sv.substr(start, len+1));

        return v;
    }

    inline const std::vector<std::string> ExplodeCopy(const std::string& s, const char& c)
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

    //deprecated
    template<typename T>
    std::string ToString(const T& key)
    {
        if constexpr(std::is_same_v<T,std::string>)
            return key;

        std::stringstream sstr;
        sstr << key;
        return sstr.str();
    }

    //deprecated
    template<typename T>
    std::wstring ToWString(const T& key)
    {
        if constexpr(std::is_same_v<T,std::wstring>)
            return key;

        std::wstringstream sstr;
        sstr << key;
        return sstr.str();
    }

    struct StringLessThan
    {
        using is_transparent = void; //to mark this struct as transparent, we need this type.
        template<typename LHS, typename RHS>
        bool operator()(const LHS& lhs, const RHS& rhs) const
        {
            return lhs < rhs;
        }
    };
}

namespace std
{
    template <> struct hash<Xroads::SmallString<32>>
    {
        size_t operator()(const Xroads::SmallString<32>& x) const
        {
            return std::hash<std::string_view>()(std::string_view(x));
        }
    };
    template <> struct hash<Xroads::SmallString<16>>
    {
        size_t operator()(const Xroads::SmallString<16>& x) const
        {
            return std::hash<std::string_view>()(std::string_view(x));
        }
    };
}

consteval inline Xroads::SmallString<32> operator""_sm(const char* ptr, size_t sz)
{
    return Xroads::SmallString<32>{std::string_view{ptr,sz}};
}
consteval inline Xroads::SmallString<16> operator""_sm16(const char* ptr, size_t sz)
{
    return Xroads::SmallString<16>{std::string_view{ptr,sz}};
}
