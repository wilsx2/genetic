#include "ga.h"
#include <cstdlib>
#include <iostream>

void print_pop(GeneticAlgorithm<int>& ga)
{
    std::cout << "Loaded: Generation " << ga.getGeneration() << "\n";
    const auto& pop = ga.getPopulation();
    for(int i = 0; i < 10 && i < pop.size(); ++i)
    {
        auto& pair = pop[i];
        std::cout << (pair.first) << " | " << pair.second << "\n";
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