#include "selection.h"
#include <utility>
#include <cstdlib>

template<typename T, std::size_t N>
T& selection::tournament(std::vector<Member<T>>& population)
{
    int i = rand() % population.size();

    std::pair<T&, float> fittest = {
        population[i].value, 
        population[i].fitness
    };

    for (int k = 1; k < N; ++k)
    {
        int j = rand() % population.size();
        if (population[j].fitness > fittest.second)
            fittest = {population[j].value, population[j].fitness};
    }

    return fittest.first;
}

// template<typename T>
// T& selection::roulette(std::vector<Member<T>>& population)
// {
//     // Assumes non-negative fitness
//     float spin = population.total_fitness * (static_cast<float>(rand()) / static_cast<float>(RAND_MAX));
//     int i = 0;
//     while(spin > population[i].fitness)
//     {
//         spin -= population[i].fitness;
//         ++i;
//     }
//     return population[i].value;
// }

template<typename T>
T& selection::rankBased(std::vector<Member<T>>& population)
{
    auto size = population.size();
    int total_rank = size*(size+1)/2;

    int spin = (rand() % (total_rank)) + 1;
    int i = 0;
    while(spin > size - i)
    {
        spin -= size - i;
        ++i;
    }
    return population[i].value;
}