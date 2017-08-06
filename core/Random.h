#ifndef RANDOM_H
#define RANDOM_H

#include <chrono>
#include <random>

using ulong = unsigned long;


class RandomProperties
{
public:
    static ulong defaultSeed();
    static ulong randomSeed();
};


class UniformInteger
{
public:
    UniformInteger();

    void setSeed(ulong seed);

    int operator()(int min, int max) const;
    int operator()() const;

private:
    mutable std::mt19937                       m_generator;
    mutable std::uniform_int_distribution<int> m_uniformDistribution;
};


class UniformReal
{
public:
    UniformReal();

    void setSeed(ulong seed);

    double operator()(double min, double max) const;
    double operator()() const;

private:
    mutable std::mt19937                           m_generator;
    mutable std::uniform_real_distribution<double> m_uniformDistribution;
};


#endif // RANDOM_H
