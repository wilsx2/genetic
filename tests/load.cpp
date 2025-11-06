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
        .1f,
        selection::tournament<int, 5>
    );

    ga.load_population("populations/saveload");
    print_pop<int>(ga, 5, [](int n){ return std::to_string(n); });
}