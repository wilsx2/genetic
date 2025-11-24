#ifndef SELECTION_H
#define SELECTION_H

#include "core/member.h"
#include "utils/rng.h"
#include <vector>
#include <functional>

namespace selection
{
template <typename T>
using Func = std::function<const T&(const std::vector<Member<T>>&, RNG& rng)>;

template<typename T, std::size_t N>
const T& tournament(const std::vector<Member<T>>& population, RNG& rng);

template<typename T>
const T& rankBased(const std::vector<Member<T>>& population, RNG& rng);

}

#include "selection.tpp"
#endif