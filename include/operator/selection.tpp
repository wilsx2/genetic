#include "selection.h"
#include <utility>
#include <stdexcept>

template<typename T, std::size_t N>
const T& selection::tournament(const Generation<T>& generation, RNG& rng)
{
    int i = rng.index(generation.size());

    int fittest_i = i;
    
    for (int k = 1; k < N; ++k)
    {
        int j = rng.index(generation.size());
        if (generation[j].fitness > generation[fittest_i].fitness)
            fittest_i = j;
    }

    return generation[fittest_i].value;
}

template<typename T>
const T& selection::rankBased(const Generation<T>& generation, RNG& rng)
{
    auto size = generation.size();
    int total_rank = size*(size+1)/2;

    int spin = rng.integer(1, total_rank);
    int i = 0;
    while(spin > i + 1)
    {
        spin -= (i + 1);
        ++i;
    }
    return generation[i].value;
}

template<typename T>
const T& selection::roulette(const Generation<T>& generation, RNG& rng)
{
    if (generation.lowestScore() < 0.f)
        throw std::invalid_argument("Generation cannot have negative fitness scores");

    int spin = rng.real(0.f, generation.totalFitness());
    int i = 0;
    while(spin > generation[i].fitness)
    {
        spin -= generation[i].fitness;
        ++i;
    }
    return generation[i].value;
}