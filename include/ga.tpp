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
    float elitism_rate
)   : fitness_(fitness)
    , mutate_(mutate)
    , crossover_(crossover)
    , elitism_rate_(elitism_rate)
    , generation_(1)
{
    srand(std::time(0));

    population_.reserve(population_size);
    for (int i = 0; i < population_size; ++i)
    {
        auto child = birth();
        population_.emplace_back(child, fitness_(child)); // Note: Assumes copy constructor
    }
    sortPopulation();
}

template <typename T>
void GeneticAlgorithm<T>::sortPopulation()
{
    std::sort(population_.begin(), population_.end(), [](const std::pair<T, float>& a, const std::pair<T, float>& b) {
        return a.second > b.second;
    });
}

template <typename T>
template <std::size_t N>
T& GeneticAlgorithm<T>::tournamentSelect()
{
    int i = rand() % population_.size();

    std::pair<T&, float> fittest = {population_[i].first, population_[i].second};
    for (int k = 1; k < N; ++k)
    {
        int j = rand() % population_.size();
        if (population_[j].second > fittest.second)
            fittest = population_[j];
    }
    return fittest.first;
}

template <typename T>
void GeneticAlgorithm<T>::evolve()
{
    std::vector<std::pair<T, float>> new_population;

    // Apply Elitism
    for(int i = 0; i < population_.size() * elitism_rate_; ++i)
    {
        new_population.push_back(population_[i]);
    }
    
    // Populate with children
    while (new_population.size() < population_.size())
    {
        // Select
        T& parent_a = tournamentSelect<10>(); //TODO: parameterize
        T& parent_b = tournamentSelect<10>();

        // Crossover
        T offspring = crossover_(parent_a, parent_b);

        // Mutate
        mutate_(offspring);
        
        new_population.emplace_back(offspring, fitness_(offspring));
    }

    // Overwrite old population
    population_ = new_population;
    sortPopulation();
    ++generation_;
}

template <typename T>
void GeneticAlgorithm<T>::evolve(std::size_t n)
{
    for (int i = 0; i < n; ++i) evolve();
}

template <typename T>
void GeneticAlgorithm<T>::evolve_until_fitness(float target)
{
    while(population_[0].second < target) evolve();
}

template <typename T>
const std::vector<std::pair<T,float>>& GeneticAlgorithm<T>::getPopulation()
{
    return population_;
}

template <typename T>
std::size_t GeneticAlgorithm<T>::getGeneration()
{
    return generation_;
}

template <typename T>
bool GeneticAlgorithm<T>::save_population(std::string filepath)
{
    /*
        NOTE: In the future should not take filepath as a parameter. Should instead output with the format of:
            Description of data in plaintext, retrieved from source type ideally.
            Identifier (hash) of the population, generated when first created and stored in GAs using that population
            Current generation of that population
    */
    std::ofstream output (filepath);
    
    if (!output.is_open())
        return false;
    
    // Current Generation
    output.write(reinterpret_cast<char*>(&generation_), sizeof(std::size_t));

    // Population Size
    auto size = population_.size();
    output.write(reinterpret_cast<char*>(&size), sizeof(std::size_t));

    // Population
    output.write(reinterpret_cast<char*>(population_.data()), sizeof(T)*population_.size());

    output.close();
    return true;
}

template <typename T>
bool GeneticAlgorithm<T>::load_population(std::string filepath)
{
    std::ifstream input (filepath);
    
    if (!input.is_open())
        return false;

    // Current Generation
    input.read(reinterpret_cast<char*>(&generation_), sizeof(std::size_t)); 

    // Population Size
    std::size_t size;
    input.read(reinterpret_cast<char*>(&size), sizeof(std::size_t)); 

    // Population
    population_.resize(size); // NOTE: Assumes default constructor, could be implemented with a for loop
    input.read(reinterpret_cast<char*>(population_.data()), sizeof(T)*population_.size()); 

    input.close();
    return true;
}