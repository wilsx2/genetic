#include "generation.h"

template <typename T>
Generation<T>::Generation(std::vector<Member<T>>&& members)
    : members_(members)
{
    members_.sort(members.begin(), members.end());
}

template <typename T>
const Member<T>& Generation<T>::operator[](std::size_t index) const
{
    return members_[index];
}

template <typename T>
const std::vector<Member<T>>& Generation<T>::data() const
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
    fittest().fitness;
}