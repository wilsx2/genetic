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
    population_.members.reserve(population_size);
    for (int i = 0; i < population_size; ++i)
    {
        population_.add(birth());
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
    population_.best_of.push_back(population_.fittest());
}

template <typename T>
void GeneticAlgorithm<T>::evolve()
{
    std::vector<T> new_members;
    
    // Populate with children
    while (new_members.size() < population_.size() - numElites())
    {
        // Select
        T& parent_a = select_(population_);
        T& parent_b = select_(population_);

        // Crossover
        T offspring = crossover_(parent_a, parent_b);

        // Mutate
        mutate_(offspring);
        
        new_members.push_back(offspring);
    }

    // Overwrite old population
    population_.newGeneration(new_members);
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
bool GeneticAlgorithm<T>::savePopulation(std::string label)
{
    return population_.save(label);
}

template <typename T>
bool GeneticAlgorithm<T>::loadPopulation(std::string filename)
{
    return population_.load(filename);
}