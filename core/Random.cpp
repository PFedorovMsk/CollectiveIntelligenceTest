#include "Random.h"
#include <climits>


// RandomProperties:

ulong RandomProperties::defaultSeed()
{
    static ulong seed = std::numeric_limits<ulong>::max() / 2;
    return seed;
}

ulong RandomProperties::randomSeed()
{
    ulong seed = ulong(std::chrono::system_clock::now().time_since_epoch().count());
    return seed;
}


// UniformInteger:

UniformInteger::UniformInteger()
{
}

void UniformInteger::setSeed(ulong seed)
{
    m_generator.seed(seed);
}

int UniformInteger::operator()(int min, int max) const
{
    auto params = std::uniform_int_distribution<int>::param_type(min, max);
    m_uniformDistribution.param(params);

    return m_uniformDistribution(m_generator);
}

int UniformInteger::operator()() const
{
    return (*this)(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}


// UniformReal:

UniformReal::UniformReal()
{
}

void UniformReal::setSeed(ulong seed)
{
    m_generator.seed(seed);
}

double UniformReal::operator()(double min, double max) const
{
    auto params = std::uniform_real_distribution<double>::param_type(min, max);
    m_uniformDistribution.param(params);

    return m_uniformDistribution(m_generator);
}

double UniformReal::operator()() const
{
    return (*this)(std::numeric_limits<double>::lowest(), std::numeric_limits<double>::max());
}
