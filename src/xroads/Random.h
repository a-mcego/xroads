#pragma once

#include <random>
#include <concepts>
#include <span>

namespace Xroads
{
    void SeedRandom(u32 n);
    u32 Random();
    u32 Random(u32 low, u32 high); //range inclusive
    double RandomDouble();
    double RandomDouble(double low, double high);
    float RandomFloat();
    float RandomFloat(float low, float high);

    template<std::floating_point FP>
    int WeightedChoice(const std::span<FP>& weights)
    {
        extern std::mt19937 generator;
        if (weights.empty())
            Kill("weights empty");
        std::discrete_distribution<> disc_dist(weights.begin(), weights.end());
        return disc_dist(generator);
    }

    //in, a,b ]0,1]
    //out, two normally distributed values
    std::pair<double,double> RandomNormal(double a, double b, double sigma);
    double RandomNormal(double sigma=1.0);

    //from https://vcg.isti.cnr.it/activities/OLD/geometryegraphics/pointintetraedro.html
    inline C3 RandomPointInTetrahedron(const C3& p1, const C3& p2, const C3& p3, const C3& p4)
    {
        double s = RandomDouble();
        double t = RandomDouble();
        double u = RandomDouble();
        if(s+t>1.0)
        {
            s = 1.0 - s;
            t = 1.0 - t;
        }
        if(t+u>1.0)
        {
            double tmp = u;
            u = 1.0 - s - t;
            t = 1.0 - tmp;

        }
        else if(s+t+u>1.0)
        {
            double tmp = u;
            u = s + t + u - 1.0;
            s = 1.0 - t - tmp;
        }
        double a = 1.0-s-t-u;
        return p1*a + p2*s + p3*t + p4*u;
    }
}
