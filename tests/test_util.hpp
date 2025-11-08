#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include "ga.h"
#include <functional>

template <typename T>
void printCurrentGeneration(GeneticAlgorithm<T>& ga, std::size_t num_print, std::function<std::string(const T&)> to_string)
{
    std::cout << "Generation " << ga.getGeneration() << "\n";

    const std::vector<Member<T>>& pop = ga.getPopulation();
    for(int i = 0; i < num_print && i < pop.size(); ++i)
    {
        const Member<T>& member = pop[i];
        std::cout << to_string(member.value) << " | " << member.fitness << "\n";
    }
}

template <typename T>
void printBestOf(GeneticAlgorithm<T>& ga, std::function<std::string(const T&)> to_string)
{
    const std::vector<Member<T>>& best = ga.getBestOfEachGeneration();
    for(int i = 0; i < best.size(); ++i)
    {
        auto member = best[i];
        std::cout << i <<" | " << to_string(member.value) << "\n";
    }
}

#endif