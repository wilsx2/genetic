#include "ga.h"

template <typename T>
Genetic<T>::Genetic(
    std::size_t population_size,
    std::function<T()> birth,
    std::function<float(T&)> fitness,
    std::function<void(T&)> mutate,
    std::function<void(T&,T&)> crossover,
    float mutation_rate,
    float crossover_rate,
    float elitism_rate
)   : fitness_(fitness)
    , mutate_(mutate)
    , crossover_(crossover)
    , mutation_rate_(mutation_rate)
    , crossover_rate_(crossover_rate)
    , elitism_rate_(elitism_rate)
{
    for (int i = 0; i < population_size; ++i)
    {
        population_.emplace_back(birth());
    }
    fitness_scores_.resize(population_size);
}