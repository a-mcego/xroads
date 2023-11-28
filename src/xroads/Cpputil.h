#pragma once

namespace Xroads
{
    template<typename T>
    struct Tracked
    {
        bool changed=true;
        T data{};

        Tracked(const T& data_):data(data_){}

        void Set(const T& data_)
        {
            changed = (changed || (data_ != data));
            data = data_;
        }
        const T& Get()
        {
            return data;
        }
        bool IsChanged()
        {
            bool ret = changed;
            changed = false;
            return ret;
        }
    };


    template<typename T, typename Comparator>
    struct BestValue
    {
        bool inited=false;
        T data{0};

        BestValue():data(T(0)) {}

        BestValue(const T& data_):data(data_){}

        void Set(const T& data_)
        {
            if (!inited)
            {
                data = data_;
                inited = true;
            }
            else if (Comparator()(data_,data))
            {
                data = data_;
            }
        }
        T Get()
        {
            return data;
        }

        void Reset(T newdata)
        {
            data = newdata;
        }
    };

    template<typename T> using MaxValue = BestValue<T,std::greater<T>>;
    template<typename T> using MinValue = BestValue<T,std::less<T>>;

    struct MinMaxCoord
    {
        MaxValue<f32> maxx, maxy;
        MinValue<f32> minx, miny;

        void Add(const Coord2D<f32>& f)
        {
            maxx.Set(f.x);
            minx.Set(f.x);
            maxy.Set(f.y);
            miny.Set(f.y);
        }
    };


    template<typename T>
    struct StaticHandler
    {
        StaticHandler() { T::Init(); }
        ~StaticHandler() { T::Quit(); }
    };
}
