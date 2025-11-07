#include "test_util.hpp"
#include "ga.h"
#include <cstdlib>
#include <iostream>

int main()
{
    srand(std::time(0));

    GeneticAlgorithm<int> ga (
        10,
        [](){return rand();},
        [](int&){return 0.f; },
        [](int&){return;},
        [](int&, int&){return rand();},
        .0f,
        selection::tournament<int, 5>
    );

    ga.evolve(rand() % 91 + 10);
    printCurrentGeneration<int>(ga, 5, [](int n){ return std::to_string(n); });
    printBestOf<int>(ga, [](int n){ return std::to_string(n); });
    ga.savePopulation("saveload");
}