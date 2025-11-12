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

template<typename T>
T& selection::rankBased(std::vector<Member<T>>& population)
{
    auto size = population.size();
    int total_rank = size*(size+1)/2;

    int spin = (rand() % (total_rank)) + 1;
    int i = 0;
    while(spin > i + 1)
    {
        spin -= (i + 1);
        ++i;
    }
    return population[i].value;
}