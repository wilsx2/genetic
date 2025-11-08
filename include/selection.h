#ifndef SELECTION_H
#define SELECTION_H

#include "member.h"
#include <vector>
#include <functional>

namespace selection
{
template <typename T>
using Func = std::function<T&(std::vector<Member<T>>&)>;

template<typename T, std::size_t N>
T& tournament(std::vector<Member<T>>& population);

template<typename T>
T& rankBased(std::vector<Member<T>>& population);

}

#include "selection.tpp"
#endif