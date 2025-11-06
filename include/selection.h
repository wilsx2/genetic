#ifndef SELECTION_H
#define SELECTION_H

#include "population.h"

namespace selection
{

template<typename T, std::size_t N>
T& tournament(Population<T>& population);

}

#include "selection.tpp"
#endif