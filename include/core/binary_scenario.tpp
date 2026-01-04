#include "binary_scenario.h"

namespace genetic 
{

template <typename T>
BinaryEncodedScenario<T>::BinT BinaryEncodedScenario<T>::birth(util::RNG& rng)
{
    return BinT::birth(rng);
}

template <typename T>
BinaryEncodedScenario<T>::BinT BinaryEncodedScenario<T>::crossover(const BinT& a, const BinT& b, util::RNG& rng)
{
    return BinT::crossover(a, b, rng);
}

template <typename T>
void BinaryEncodedScenario<T>::mutate(BinT& a, util::RNG& rng)
{
    return BinT::mutate(a, rng);
}

}