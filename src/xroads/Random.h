#pragma once

namespace Xroads
{
    void SeedRandom(u32 n);
    u32 Random();
    u32 Random(u32 low, u32 high); //range inclusive
    double RandomDouble();
    double RandomDouble(double low, double high);
    int WeightedChoice(const std::vector<double>& weights);

    //in, a,b ]0,1]
    //out, two normally distributed values
    std::pair<double,double> RandomNormal(double a, double b, double sigma);
    double RandomNormal(double sigma=1.0);
}
