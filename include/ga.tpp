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
        auto child = birth();
        population_.emplace_back(child, fitness_(child));
    }
    std::sort(population_.begin(), population_.end(), [](const std::pair<T, float>& a, const std::pair<T, float>& b) {
        return a.second < b.second;
    });
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
        std::uniform_real_distribution<float> chance(0.f, 1.f);
        if(chance(gen_) < mutation_rate_)
            mutate_(offspring);
        new_population.emplace_back(offspring, fitness_(offspring));
    }

    population_ = new_population;
    std::sort(population_.begin(), population_.end(), [](const std::pair<T, float>& a, const std::pair<T, float>& b) {
        return a.second < b.second;
    });
}

template <typename T>
const std::vector<std::pair<T,float>>& Genetic<T>::getPopulation()
{
    return population_;
}