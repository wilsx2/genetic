#ifndef SELECTION_H
#define SELECTION_H

#include "core/generation.h"
#include "utils/rng.h"
#include <vector>
#include <functional>

namespace genetic 
{

namespace selection
{
template <typename T>
using Func = std::function<const T&(const Generation<T>&, util::RNG& rng)>;

template<typename T, std::size_t N>
const T& tournament(const Generation<T>& generation, util::RNG& rng);

template<typename T>
const T& rankBased(const Generation<T>& generation, util::RNG& rng);

template<typename T>
const T& roulette(const Generation<T>& generation, util::RNG& rng);

}

}

#include "selection.tpp"
#endif