#include "test_util.hpp"
#include "ga.h"
#include <cstdlib>
#include <iostream>

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
    print_pop<int>(ga, 5, [](int n){ return std::to_string(n); });
    ga.save_population("populations/saveload");
}