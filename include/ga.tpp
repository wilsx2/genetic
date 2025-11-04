#include "ga.h"
#include <random>
#include <cassert>
#include <ctime>

template <typename T>
Genetic<T>::Genetic(
    std::size_t population_size,
    std::function<T()> birth,
    std::function<float(T&)> fitness,
    std::function<void(T&)> mutate,
    std::function<T(T&,T&)> crossover,
    float mutation_rate,
    float elitism_rate
)   : fitness_(fitness)
    , mutate_(mutate)
    , crossover_(crossover)
    , mutation_rate_(mutation_rate)
    , elitism_rate_(elitism_rate)
{
    gen_.seed(std::time(0));

    population_.reserve(population_size);
    for (int i = 0; i < population_size; ++i)
    {
        population_.emplace_back(birth(), 0.f);
    }
}

template <typename T>
T& Genetic<T>::rouletteSelect()
{
    std::uniform_real_distribution<float> wheel(0.f, total_fitness_);
    float spin = wheel(gen_);
    int i = 0;
    while(spin > population_[i].second)
    {
        spin -= population_[i].second;
        ++i;
    }
    return population_[i].first;
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
    // Evaluate fitness
    total_fitness_ = 0.f;
    for(auto& member : population_)
    {
        member.second = fitness_(member.first);
        total_fitness_ += member.second;
    }

    // Create new population
    std::vector<std::pair<T, float>> new_population;

    /// Apply Elitism
    // TOOD
    
    /// Populate
    while (new_population.size() < population_.size())
    {
        // Select
        T& parent_a = tournamentSelect<10>();
        T& parent_b = tournamentSelect<10>();

        // Crossover
        T offspring = crossover_(parent_a, parent_b);

        // Mutate
        std::uniform_real_distribution<float> chance(0.f, 1.f);
        if(chance(gen_) < mutation_rate_)
            mutate_(offspring);
        new_population.emplace_back(offspring, 0.f);
    }

    population_ = new_population;
}

template <typename T>
const std::vector<std::pair<T,float>>& Genetic<T>::getPopulation()
{
    return population_;
}