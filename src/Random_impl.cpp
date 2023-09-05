#include <cmath>
#include <utility>
#include <string>
#include <random>

#include "xroads/Xroads.h"

namespace Xroads
{
    //parameters a and b must contain a (random) number between ]0,1]
    //yes, 0 not included, 1 included
    std::pair<double,double> RandomNormal(double a, double b, double sigma)
    {
        const double TWO_PI = 2.0*3.14159265358979323846;

        double c = sqrt(-2.0 * log(a)) * cos(TWO_PI * b) * sigma;
        double d = sqrt(-2.0 * log(a)) * sin(TWO_PI * b) * sigma;

        return std::make_pair(c,d);
    }
    double RandomNormal(double sigma)
    {
        double a = double(u64(Random())+1ULL)/4294967297.0;
        double b = double(u64(Random())+1ULL)/4294967297.0;
        return RandomNormal(a,b,sigma).first;
    }


    std::mt19937 generator;
    std::uniform_int_distribution<u32> distribution;

    void SeedRandom(u32 n)
    {
        generator.seed(n);
    }

    u32 Random()
    {
        return distribution(generator);
    }

    u32 Random(u32 low, u32 high)
    {
        return Random()%(high-low+1)+low;
    }

    int WeightedChoice(const std::vector<double>& weights)
    {
        std::discrete_distribution<> disc_dist(weights.begin(), weights.end());
        return disc_dist(generator);
    }

    double RandomDouble()
    {
        return Random()*2.3283064365386962890625e-10;
    }

    double RandomDouble(double low, double high)
    {
        return RandomDouble()*(high-low)+low;
    }

}

