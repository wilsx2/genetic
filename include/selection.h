#ifndef SELECTION_H
#define SELECTION_H

#include "population.h"
#include <functional>

namespace selection
{
template <typename T>
using Func = std::function<T&(Population<T>&)>;

template<typename T, std::size_t N>
T& tournament(Population<T>& population);

}

#include "selection.tpp"
#endif