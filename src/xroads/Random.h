#pragma once

#include <random>
#include <concepts>
#include <span>
#include <ranges>

namespace Xroads
{
    void SeedRandom(u32 n);
    u32 Random();
    u32 Random(u32 low, u32 high); //range inclusive
    f64 RandomDouble();
    f64 RandomDouble(f64 low, f64 high);
    f32 RandomFloat();
    f32 RandomFloat(f32 low, f32 high);

    template<typename T>
    i32 WeightedChoiceFromData(const std::span<T>& data)
    {
        extern std::mt19937 generator;
        auto GetWeight = [](const T& item)->f64 {
            return item.weight;
        };
        auto range = data | std::views::transform(GetWeight);
        std::discrete_distribution<> disc_dist(range.begin(), range.end());
        return disc_dist(generator);
    }

    template<std::floating_point FP>
    i32 WeightedChoice(const std::span<FP>& weights)
    {
        extern std::mt19937 generator;
        if (weights.empty())
            Kill("weights empty");
        std::discrete_distribution<> disc_dist(weights.begin(), weights.end());
        return disc_dist(generator);
    }

    //in, a,b ]0,1]
    //out, two normally distributed values
    std::pair<f64,f64> RandomNormal(f64 a, f64 b, f64 sigma);
    f64 RandomNormal(f64 sigma=1.0);

    //from https://vcg.isti.cnr.it/activities/OLD/geometryegraphics/pointintetraedro.html
    inline C3 RandomPointInTetrahedron(const C3& p1, const C3& p2, const C3& p3, const C3& p4)
    {
        f64 s = RandomDouble();
        f64 t = RandomDouble();
        f64 u = RandomDouble();
        if(s+t>1.0)
        {
            s = 1.0 - s;
            t = 1.0 - t;
        }
        if(t+u>1.0)
        {
            f64 tmp = u;
            u = 1.0 - s - t;
            t = 1.0 - tmp;

        }
        else if(s+t+u>1.0)
        {
            f64 tmp = u;
            u = s + t + u - 1.0;
            s = 1.0 - t - tmp;
        }
        f64 a = 1.0-s-t-u;
        return p1*a + p2*s + p3*t + p4*u;
    }
}
