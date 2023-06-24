#pragma once

#include <vector>
#include <array>
#include <utility>

namespace Xroads
{
    //access-safe version of std::vector
    template<typename T>
    struct Vector
    {
        std::vector<T> v;

        auto begin() { return v.begin(); }
        auto end() { return v.end(); }
        auto size() { return v.size(); }
        auto empty() { return v.empty(); }
        void clear() { v.clear(); }
        void push_back(const T& thing) { v.push_back(thing); }


        Vector() = default;
        Vector(int size, const T& val)
        { v = vector<T>(size, val); }

        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& operator[](INDEXTYPE n) { return at<INDEXTYPE>(n); }
        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& at(INDEXTYPE n)
        {
            if (n < 0 || n >= size())
#ifdef XR_VERBOSE_ERRORS
                std::cout << "Invalid vector access with index " << n << ", size " << size() << std::endl;
#else
                std::abort();
#endif
            return v.at(n);
        }

        int Sum()
        {
            int sum=0;
            for(int i=0; i<v.size(); ++i)
                sum += v.at(i);
            return sum;
        }
    };

    template<typename T>
    struct Vector2D
    {
    private:
        int dims[2] = {};
        std::vector<T> data;

    public:
        Vector2D() = default;
        Vector2D(int Y, int X, const T& t = T())
        {
            data.assign(Y*X, t);
            dims[0] = Y, dims[1] = X;
        }

        auto begin() { return data.begin(); }
        auto end() { return data.end(); }
        auto cbegin() const { return data.cbegin(); }
        auto cend() const { return data.cend(); }

        T& operator()(int y, int x) { return data[y*dims[0]+x]; }
        const T& operator()(int y, int x) const { return data[y*dims[0]+x]; }

        T& operator()(const auto& p) { auto& [y,x] = p; return operator()(y,x); };
        const T& operator()(const auto& p) const { auto& [y,x] = p; return operator()(y,x); };

        int shape(int n) const {return dims[n];}

        bool in_bounds(const auto& p) const
        {
            auto& [y,x] = p;
            return in_bounds(y,x);
        }
        bool in_bounds(int y, int x) const
        {
            if (y<0 or y>=dims[0])
                return false;
            if (x<0 or x>=dims[1])
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

        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& operator[](INDEXTYPE n) { return at<INDEXTYPE>(n); }
        template<typename INDEXTYPE> requires std::same_as<int,INDEXTYPE>
        T& at(INDEXTYPE n)
        {
            if (n < 0 || n >= size())
#ifdef XR_VERBOSE_ERRORS
                std::cout << "Invalid array access with index " << n << ", size " << size() << std::endl;
#else
                std::abort();
#endif
            return v.at(n);
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

		U& operator[](const T& item)
		{
			auto it = find(item);
			if (it == data.end())
			{
				insert(make_pair(item, U()));
				return data.back().second;
			}
			return it->second;
		}
		typename Data::iterator find(const T& key)
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
        std::array<T, std::size_t(ENUM::N)> arr{};
        T& operator[](ENUM index) { return arr[size_t(index)]; }
        const T& operator[](ENUM index) const { return arr[size_t(index)]; }
        auto begin() { return std::begin(arr); }
        auto end() { return std::end(arr); }
    };
}
