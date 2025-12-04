#ifndef SCENARIO_H
#define SCENARIO_H

#include "serialization/serializer.h"
#include "utils/rng.h"
#include <string>

namespace genetic
{

template <typename T>
class Scenario
{
    public:
    virtual const std::string& getName() = 0;
    virtual const Serializer<T>& getSerializer() = 0;
    
    virtual float evaluateFitness(const T&) = 0;
    virtual T birth(util::RNG&) = 0;
    virtual T crossover(const T&, const T&, util::RNG&) = 0;
    virtual void mutation(T&, util::RNG&) = 0;
};

}

#endif