#include "test_util.hpp"
#include "ga.h"
#include <cstdlib>
#include <climits>
#include <iostream>

int main()
{
    GeneticAlgorithm<int> ga (
        10000,
        [](){return rand();},
        [](int& n){return -abs(1000 - n); },
        [](int& n){return n += (rand()%20000) - 10000; },
        [](int& a, int& b){return a/2 + b/2; },
        .1f,
        selection::tournament<int, 5>
    );
    print_pop<int>(ga, 5, [](int n){ return std::to_string(n); });
    ga.evolve_until_fitness(0.f);
    print_pop<int>(ga, 5, [](int n){ return std::to_string(n); });
}
