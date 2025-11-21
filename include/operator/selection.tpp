#include "selection.h"
#include <utility>

template<typename T, std::size_t N>
T& selection::tournament(std::vector<Member<T>>& population, RNG& rng)
{
    int i = rng.index(population.size());

    int fittest_i = i;
    
    for (int k = 1; k < N; ++k)
    {
        int j = rng.index(population.size());
        if (population[j].fitness > population[fittest_i].fitness)
            fittest_i = j;
    }

    return population[fittest_i].value;
}

template<typename T>
T& selection::rankBased(std::vector<Member<T>>& population, RNG& rng)
{
    auto size = population.size();
    int total_rank = size*(size+1)/2;

    int spin = rng.integer(1, total_rank);
    int i = 0;
    while(spin > i + 1)
    {
        spin -= (i + 1);
        ++i;
    }
    return population[i].value;
}