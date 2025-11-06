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
        population_.members.push_back({child, 0.f});
    }

    evaluateFitness();
}

template <typename T>
inline std::size_t GeneticAlgorithm<T>::numElites()
{
    return population_.size() * elitism_rate_;
}

template <typename T>
void GeneticAlgorithm<T>::evaluateFitness()
{
    population_.total_fitness = 0.f;
    std::size_t start = population_.generation == 1 ? 0 : numElites();
    for (int i = start; i < population_.size(); ++i)
    {
        auto score = fitness_(population_[i].value);
        population_[i].fitness = score;
        population_.total_fitness += score;
    }

    population_.sort();
}

template <typename T>
void GeneticAlgorithm<T>::evolve()
{
    std::vector<Member<T>> new_members;

    // Apply Elitism
    for (int i = 0; i < numElites(); ++i)
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
        
        new_members.push_back({offspring, 0.f});
    }

    // Overwrite old population
    population_.members = new_members;
    ++population_.generation;

    evaluateFitness();
}

template <typename T>
void GeneticAlgorithm<T>::evolve(std::size_t n)
{
    for (int i = 0; i < n; ++i) evolve();
}

template <typename T>
void GeneticAlgorithm<T>::evolveUntilFitness(float target)
{
    while(population_.fittest().fitness < target) evolve();
}

template <typename T>
const Population<T>& GeneticAlgorithm<T>::getPopulation() const
{
    return population_;
}

template <typename T>
bool GeneticAlgorithm<T>::savePopulation(std::string filepath)
{
    return population_.save(filepath);
}

template <typename T>
bool GeneticAlgorithm<T>::loadPopulation(std::string filepath)
{
    return population_.load(filepath);
}