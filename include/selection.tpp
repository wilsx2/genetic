#include "selection.h"
#include <utility>
#include <cstdlib>

template<typename T, std::size_t N>
T& selection::tournament(Population<T>& population)
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