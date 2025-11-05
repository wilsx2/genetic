#include "ga.h"
#include <cstdlib>
#include <climits>
#include <iostream>

void print_pop(GeneticAlgorithm<int>& ga)
{
    const auto& pop = ga.getPopulation();
    std::cout << "Generation " << pop.generation << "\n";
    for(int i = 0; i < 5 && i < pop.size(); ++i)
    {
        auto member = pop[i];
        std::cout << (member.value) << " | " << member.fitness << "\n";
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
