#include "generation.h"
#include <stdexcept>

namespace genetic 
{

template <typename T>
Generation<T>::Generation(std::vector<Member<T>>&& members)
    : members_(members)
    , total_fitness_(0.f)
{
    if (members.size() == 0)
        throw std::invalid_argument("Generation size must be greater than 0");

    std::sort(members.begin(), members.end());

    for (Member<T>& member : members)
    {
        total_fitness_ += member.fitness;
    }
}

template <typename T>
const Member<T>& Generation<T>::operator[](std::size_t index) const
{
    return members_[index];
}

template <typename T>
const std::vector<Member<T>>& Generation<T>::members() const
{
    return members_;
}

template <typename T>
std::size_t Generation<T>::size() const
{
    return members_.size();
}

template <typename T>
const Member<T>& Generation<T>::fittest() const
{
    return members_.back();
}

template <typename T>
float Generation<T>::fittestScore() const
{
    return fittest().fitness;
}

template <typename T>
float Generation<T>::lowestScore() const
{
    return members_[0].fitness;
}

template <typename T>
float Generation<T>::totalFitness() const
{
    return total_fitness_;
}

}