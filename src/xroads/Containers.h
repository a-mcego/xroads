#pragma once

#include <vector>
#include <array>
#include <utility>
#include <unordered_map>
#include <map>

#include "Strutil.h"
#include "Option.h"

namespace Xroads
{
    //access-safe version of std::vector
    template<typename T>
    struct Vector
    {
        using value_type = T;
        std::vector<T> v;

        auto begin() { return v.begin(); }
        auto end() { return v.end(); }
        auto begin() const { return v.begin(); }
        auto end() const { return v.end(); }
        auto cbegin() const { return v.cbegin(); }
        auto cend() const { return v.cend(); }
        auto size() const { return v.size(); }
        auto empty() const { return v.empty(); }
        void clear() { v.clear(); }
        void push_back(const T& thing) { v.push_back(thing); }

        T& front() { return at(0); }
        const T& front() const { return at(0); }
        T& back() { return at(size()-1); }
        const T& back() const { return at(size()-1); }

        Vector() = default;
        Vector(int size, const T& val)
        { v = vector<T>(size, val); }

        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& operator[](INDEXTYPE n) { return at<INDEXTYPE>(n); }

        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        const T& operator[](INDEXTYPE n) const { return at<INDEXTYPE>(n); }

        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& at(INDEXTYPE n)
        {
            if (n < 0 || n >= size())
            {
                Log("Invalid access with index " + ToString(n) + ", size " + ToString(size()));
                Kill("");
            }
            return v[n];
        }
        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        const T& at(INDEXTYPE n) const
        {
            if (n < 0 || n >= size())
            {
                Log("Invalid access with index " + ToString(n) + ", size " + ToString(size()));
                Kill("");
            }
            return v[n];
        }

        T Sum() const
        {
            T sum=0;
            for(size_t i=0; i<v.size(); ++i)
                sum += v[i];
            return sum;
        }
    };

    template<typename Key, typename Value, typename Hash = std::hash<Key>, typename KeyEqual = std::equal_to<Key>>
    struct Hashmap
    {
    private:
        std::unordered_map<Key, Value, Hash, KeyEqual> data;

    public:
        Value& GetOrInsert(const Key& key) { return data[key]; }
        OptionRef<Value> Get(const Key& key)
        {
            auto iter = data.find(key);
            if (iter == data.end())
                return {};
            return {iter->second};
        };
        OptionCRef<Value> Get(const Key& key) const
        {
            auto iter = data.find(key);
            if (iter == data.end())
                return {};
            return {iter->second};
        };
        bool Has(const Key& key) const { return data.contains(key); }
        void Clear() { data.clear(); }

        Value& operator[](const Key& key) { return GetOrInsert(key); }
        auto begin() { return data.begin(); }
        auto end() { return data.end(); }
        auto begin() const { return data.begin(); }
        auto end() const { return data.end(); }
        auto cbegin() const { return data.cbegin(); }
        auto cend() const { return data.cend(); }
    };

    template<typename T>
    struct Vector2D
    {
    private:
        Coord2D<int> dims{};
        std::vector<T> data;

    public:
        Vector2D() = default;
        Vector2D(const Coord2D<int>& dims_, const T& t = T()):dims(dims_)
        {
            data.assign(dims.x*dims.y, t);
        }

        auto X() const { return dims.x; }
        auto Y() const { return dims.y; }
        auto size() const { return dims; }

        auto begin() { return data.begin(); }
        auto end() { return data.end(); }
        auto cbegin() const { return data.cbegin(); }
        auto cend() const { return data.cend(); }

        void AssertBounds(int x, int y) const
        {
            if (!in_bounds(x,y))
            {
                Kill(std::format("{},{} doesn't fit into Vec2D {},{}", x, y, X(), Y()));
            }
        }

        T& operator()(int x, int y) { AssertBounds(x,y); return data[y*dims.x+x]; }
        const T& operator()(int x, int y) const { AssertBounds(x,y); return data[y*dims.x+x]; }

        T& operator()(const auto& p) { return operator()(p.x, p.y); };
        const T& operator()(const auto& p) const { return operator()(p.x, p.y); };

        bool in_bounds(const auto& p) const
        {
            return in_bounds(p.x, p.y);
        }
        bool in_bounds(int x, int y) const
        {
            if (x<0 or x>=dims.x)
                return false;
            if (y<0 or y>=dims.y)
                return false;
            return true;
        }
    };

    template<typename T>
    struct Vector3D
    {
    private:
        std::array<i32,3> dims{};
        std::vector<T> data;

    public:
        Vector3D() = default;
        Vector3D(int X, int Y, int Z, const T& t = T())
        {
            data.assign(X*Y*Z, t);
            dims[0] = X, dims[1] = Y, dims[2] = Z;
        }

        auto begin() { return data.begin(); }
        auto end() { return data.end(); }
        auto cbegin() const { return data.cbegin(); }
        auto cend() const { return data.cend(); }

        T& operator()(int x, int y, int z) { return data.at(z*dims[1]*dims[0]+y*dims[0]+x); }
        const T& operator()(int x, int y, int z) const { return data.at(z*dims[1]*dims[0]+y*dims[0]+x); }

        T& operator()(const auto& p) { auto& [x,y,z] = p; return operator()(x,y,z); };
        const T& operator()(const auto& p) const { auto& [x,y,z] = p; return operator()(x,y,z); };

        int shape(int n) const {return dims.at(n);}

        bool in_bounds(const auto& p) const
        {
            auto& [x,y,z] = p;
            return in_bounds(x,y,z);
        }
        bool in_bounds(int x, int y, int z) const
        {
            if (x<0 or x>=dims[0])
                return false;
            if (y<0 or y>=dims[1])
                return false;
            if (z<0 or z>=dims[2])
                return false;
            return true;
        }
    };

    //access-safe version of std::array
    template<typename T, int SIZE>
    struct Array
    {
        std::array<T,SIZE> v;

        auto begin() { return v.begin(); }
        auto end() { return v.end(); }
        auto size() { return v.size(); }
        auto empty() { return v.empty(); }

        Array() = default;
        Array(int size, const T& val)
        { v = vector<T>(size, val); }

        //FIXME: why is this requires clause here? makes no sense.
        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& operator[](INDEXTYPE n)
        {
            return at<INDEXTYPE>(n);
        }
        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& at(INDEXTYPE n)
        {
            if (n < 0 || n >= size())
            {
                Log("Invalid array access with index " + ToString(n) + ", size " + ToString(SIZE));
                Kill("");
            }
            return v[n];
        }
    };

    //This is a naive implementation of an associative container, using an unsorted std::vector instead of a red-black tree or a hashmap
    template<typename T, typename U>
    class VectorMap
    {
    private:
        using Data = std::vector<std::pair<T, U>>;

        Data data;
    public:
        void insert(const std::pair<T, U>& elem)
        {
            data.push_back(elem);
        }

        template<typename KEYPARAM>
        U& operator[](const KEYPARAM& item) requires std::same_as<T,KEYPARAM> || std::convertible_to<T,std::string_view>
        {
            auto it = find(item);
            if (it == data.end())
            {
                insert(std::make_pair(T(item), U()));
                return data.back().second;
            }
            return it->second;
        }
        template<typename KEYPARAM>
        typename Data::iterator find(const KEYPARAM& key) requires std::same_as<T,KEYPARAM> || std::convertible_to<T,std::string_view>
        {
            typename Data::iterator it = data.begin();
            for (; it != data.end(); ++it)
            {
                if (it->first == key)
                    break;
            }
            return it;
        }

        typename Data::iterator begin() { return data.begin(); }
        typename Data::iterator end()   { return data.end(); }
        bool empty()                      { return data.empty(); }
        size_t size()                     { return data.size(); }
        void clear()                      { data.clear(); }
    };

    template<typename T, typename ENUM>
    struct EnumArray
    {
        std::strong_ordering operator<=>(const EnumArray& c) const
        {
            for(int i=0; i<arr.size(); ++i)
                if (auto cmp_result = (arr[i] <=> c.arr[i]); cmp_result != 0)
                    return cmp_result;
            return std::strong_ordering::equal;
        }
        std::array<T, std::size_t(ENUM::N)> arr{};
        constexpr T& operator[](ENUM index) { return arr[size_t(index)]; }
        constexpr const T& operator[](ENUM index) const { return arr[size_t(index)]; }
        auto begin() { return std::begin(arr); }
        auto end() { return std::end(arr); }
        auto begin() const { return std::cbegin(arr); }
        auto end() const { return std::cend(arr); }
    };
}
