#pragma once

namespace Xroads
{
    void SeedRandom(u32 n);
    u32 Random();
    u32 Random(u32 low, u32 high); //range inclusive
    double RandomDouble();
    double RandomDouble(double low, double high);

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
}
