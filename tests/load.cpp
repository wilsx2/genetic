#include "ga.h"
#include <cstdlib>
#include <iostream>

void print_pop(GeneticAlgorithm<int>& ga)
{
    const auto& pop = ga.getPopulation();
    std::cout << "Loaded: Generation " << pop.generation << "\n";
    for(int i = 0; i < 5 && i < pop.size(); ++i)
    {
        auto member = pop[i];
        std::cout << (member.value) << " | " << member.fitness << "\n";
    }
}

int main()
{
    GeneticAlgorithm<int> ga (
        10,
        [](){return rand();},
        [](int&){return 0.f; },
        [](int&){return;},
        [](int&, int&){return rand();},
        .1f
    );

    ga.load_population("populations/saveload");
    print_pop(ga);
}