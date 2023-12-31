#pragma once

#include <sstream>
#include <string>
#include <functional>
#include <string_view>
#include <cstring>
#include <format>
#include "xroads/Coords.h"
#include "xroads/Concepts.h"

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

        constexpr bool startswith(std::string_view v)
        {
            if (len < v.size())
                return false;
            for(int i=0; i<v.size(); ++i)
                if (v[i] != data_internal[i])
                    return false;
            return true;
        }
    };

    using Sm16 = SmallString<16>;
    using Sm32 = SmallString<32>;

    template<size_t size, typename T>
    inline SmallString<size> ToSm(const T& str)
    {
        return SmallString<size>(std::string_view(str));
    }

    template<typename T>
    inline Sm32 ToSm32(const T& str)
    {
        return Sm32(std::string_view(str));
    }

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

    inline const std::vector<std::string_view> Explode(std::string_view s, const char& c)
    {
        std::vector<std::string_view> v;

        size_t start=0, len=0;//, i=0;
        for(auto n:s)
        {
            ++len;
            if(n == c)
            {
                v.push_back(s.substr(start, len-1));
                start += len;
                len = 0;
            }
        }
        v.push_back(s.substr(start));
        return v;
    }

    inline const std::vector<std::string> ExplodeCopy(std::string s, const char& c)
    {
        std::vector<std::string> v;

        size_t start=0, len=0;//, i=0;
        for(auto n:s)
        {
            ++len;
            if(n == c)
            {
                v.push_back(s.substr(start, len-1));
                start += len;
                len = 0;
            }
        }
        v.push_back(s.substr(start));
        return v;
    }

    template<typename T>
    T FromString(std::string_view key)
    {
        std::stringstream sstr{std::string{key}};
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

    template<size_t N>
    std::ostream& operator<<(std::ostream& o, const SmallString<N>& t)
    {
        return o << t.view();
    }
}

namespace std
{
    template <size_t SIZE> struct hash<Xroads::SmallString<SIZE>>
    {
        size_t operator()(const Xroads::SmallString<SIZE>& x) const
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
