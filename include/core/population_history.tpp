#include "population_history.h"
#include <stdexcept>
#include <algorithm>
#include <format>

template <typename T>
PopulationHistory<T>::PopulationHistory(uint32_t id, std::size_t population_size)
    : id_(id)
    , population_size_(population_size)
{
    if (population_size == 0)
        throw std::invalid_argument("Population size must be greater than 0");
}

template <typename T>
uint32_t PopulationHistory<T>::id() const
{
    return id_;
}

template <typename T>
std::string PopulationHistory<T>::formattedId() const
{
    return std::format("{:08x}", id_);
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
const Generation<T>& PopulationHistory<T>::generation(std::size_t i) const
{
    return generations_[i];
}

template <typename T>
const Generation<T>& PopulationHistory<T>::current() const
{
    if (generations_.size() == 0)
        throw std::logic_error("Cannot get information about a current generation that does not exist");

    return generations_.back();
}

template <typename T>
const std::vector<Member<T>>& PopulationHistory<T>::fittestHistory() const
{
    return fittest_history_;
}

template <typename T>
void PopulationHistory<T>::pushNext(std::vector<Member<T>>&& next)
{
    if (next.size() != population_size_)
        throw std::invalid_argument("Size " + std::to_string(next.size()) + "of next generation conflicts with size "
        + std::to_string(population_size_) + " of population history");   
    
    generations_.emplace_back(std::move(next));
    fittest_history_.push_back(generations_.back().fittest());
}

template <typename T>
void PopulationHistory<T>::restart(uint32_t new_id, std::size_t new_size)
{
    id_ = new_id;
    population_size_ = new_size;
    generations_.clear();
}

template <typename T>
float PopulationHistory<T>::currentFittestScore() const
{
    return current().fittestScore();
}