#include "ga.h"
#include <algorithm>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>

template <typename T>
GeneticAlgorithm<T>::GeneticAlgorithm(
    std::size_t population_size,
    std::function<T()> birth,
    std::function<float(T&)> fitness,
    std::function<void(T&)> mutate,
    std::function<T(T&,T&)> crossover,
    float elitism_rate,
    selection::Func<T> select
)   : fitness_(fitness)
    , mutate_(mutate)
    , crossover_(crossover)
    , elitism_rate_(elitism_rate)
    , select_(select)
    , population_()
{
    srand(std::time(0));

    population_.members.reserve(population_size);
    for (int i = 0; i < population_size; ++i)
    {
        auto child = birth();
        population_.members.push_back({child, fitness_(child)});
    }
    population_.sort();
}

template <typename T>
void GeneticAlgorithm<T>::evolve()
{
    std::vector<Member<T>> new_members;

    // Apply Elitism
    for(int i = 0; i < population_.size() * elitism_rate_; ++i)
    {
        new_members.push_back(population_[i]);
    }
    
    // Populate with children
    while (new_members.size() < population_.size())
    {
        // Select
        T& parent_a = select_(population_);
        T& parent_b = select_(population_);

        // Crossover
        T offspring = crossover_(parent_a, parent_b);

        // Mutate
        mutate_(offspring);
        
        new_members.push_back({offspring, fitness_(offspring)});
    }

    // Overwrite old population
    population_.members = new_members;
    ++population_.generation;
    population_.sort();
}

template <typename T>
void GeneticAlgorithm<T>::evolve(std::size_t n)
{
    for (int i = 0; i < n; ++i) evolve();
}

template <typename T>
void GeneticAlgorithm<T>::evolve_until_fitness(float target)
{
    while(population_.fittest().fitness < target) evolve();
}

template <typename T>
const Population<T>& GeneticAlgorithm<T>::getPopulation() const
{
    return population_;
}

template <typename T>
bool GeneticAlgorithm<T>::save_population(std::string filepath)
{
    return population_.save(filepath);
}

template <typename T>
bool GeneticAlgorithm<T>::load_population(std::string filepath)
{
    return population_.load(filepath);
}