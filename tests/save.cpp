#include "ga.h"
#include <cstdlib>
#include <iostream>

void print_pop(GeneticAlgorithm<int>& ga)
{
    std::cout << "Saving: Generation " << ga.getGeneration() << "\n";
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
        .0f
    );

    ga.evolve(rand() % 91 + 10);
    print_pop(ga);
    ga.save_population("populations/saveload");
}