#include "population_history.h"
#include <stdexcept>

template <typename T>
PopulationHistory<T>::PopulationHistory(std::size_t population_size)
    : population_size_(population_size)
{
    if (population_.size() == 0)
        throw std::invalid_argument("Population size must be greater than 0");
}

template <typename T>
std::size_t PopulationHistory<T>::populationSize() const
{
    return population_size_;
}

template <typename T>
std::size_t PopulationHistory<T>::numGenerations() const
{
    return generations_.size();
}

template <typename T>
const std::vector<std::vector<Member<T>>>& PopulationHistory<T>::getHistory() const
{
    return generations_;
}

template <typename T>
const std::vector<Member<T>>& PopulationHistory<T>::getCurrent() const
{
    if (generations_.size() == 0)
        throw std::logic_error("Cannot get information about a current generation that does not exist");

    return generations_.back();
}

template <typename T>
void PopulationHistory<T>::pushNext(std::vector<Member<T>>&& next)
{
    if (next.size() != population_size_)
        throw std::invalid_argument("Size " + std::to_string(next.size()) + "of next generation conflicts with size "
            + std::to_string(population_size_) " of population history");   
    
    generations_.emplace_back(next);
}

template <typename T>
void PopulationHistory<T>::clear(std::size_t new_size)
{
    population_size_ = new_size;
    generations_.clear();
}

template <typename T>
const Member<T>& PopulationHistory<T>::getFittest() const
{
    return getFittest(generations_.size() - 1);
}

template <typename T>
const Member<T>& PopulationHistory<T>::getFittest(std::size_t generation) const
{
    if (next.size() != population_size_)
        throw std::invalid_argument("Generation " + std::to_string(generation) + " does not exist. "
            + "The most recent generation has index " + std::to_string(generations_.size() - 1));
    return generations_.back();
}

template <typename T>
const Member<T>& PopulationHistory<T>::getFittestScore() const
{
    return getFittest().fitness;
}

template <typename T>
const Member<T>& PopulationHistory<T>::getFittestScore(std::size_t generation) const
{
    return getFittest(generation).fitness;
}