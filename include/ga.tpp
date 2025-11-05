#include "ga.h"
#include <random>
#include <algorithm>
#include <cassert>
#include <ctime>

template <typename T>
Genetic<T>::Genetic(
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
    gen_.seed(std::time(0));

    population_.reserve(population_size);
    for (int i = 0; i < population_size; ++i)
    {
        auto child = birth();
        population_.emplace_back(child, fitness_(child));
    }
    sortPopulation();
}

template <typename T>
void Genetic<T>::sortPopulation()
{
    std::sort(population_.begin(), population_.end(), [](const std::pair<T, float>& a, const std::pair<T, float>& b) {
        return a.second > b.second;
    });
}

template <typename T>
template <std::size_t N>
T& Genetic<T>::tournamentSelect()
{
    std::uniform_int_distribution<int> rand_i(0, population_.size() - 1);
    int i = rand_i(gen_);

    std::pair<T&, float> fittest = {population_[i].first, population_[i].second};
    for (int k = 1; k < N; ++k)
    {
        int j = rand_i(gen_);
        if (population_[j].second > fittest.second)
            fittest = population_[j];
    }
    return fittest.first;
}

template <typename T>
void Genetic<T>::evolve()
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
        T& parent_a = tournamentSelect<10>();
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
void Genetic<T>::evolve(std::size_t n)
{
    for (int i = 0; i < n; ++i) evolve();
}

template <typename T>
void Genetic<T>::evolve_until_fitness(float target)
{
    while(population_[0].second < target) evolve();
}

template <typename T>
const std::vector<std::pair<T,float>>& Genetic<T>::getPopulation()
{
    return population_;
}

template <typename T>
std::size_t Genetic<T>::getGeneration()
{
    return generation_;
}