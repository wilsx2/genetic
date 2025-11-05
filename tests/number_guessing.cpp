#include "ga.h"
#include <cstdlib>
#include <climits>
#include <iostream>

void print_pop(GeneticAlgorithm<int>& ga)
{
    std::cout << "Generation " << ga.getGeneration() << "\n";
    const auto& pop = ga.getPopulation();
    for(int i = 0; i < 10 && i < pop.size(); ++i)
    {
        auto& pair = pop[i];
        std::cout << pair.first << " | " << pair.second << "\n";
    }
}

int main()
{
    GeneticAlgorithm<int> ga (
        10000,
        [](){return rand();},
        [](int& n){return -abs(1000 - n); },
        [](int& n){return n += (rand()%20000) - 10000; },
        [](int& a, int& b){return a/2 + b/2; },
        .1f
    );
    print_pop(ga);
    ga.evolve_until_fitness(0.f);
    print_pop(ga);
}
