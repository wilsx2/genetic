#include "test_util.hpp"
#include "ga.h"
#include "binary_encoding.h"

#include <string>
#include <cstdlib>
#include <climits>
#include <iostream>

float fitness(BinaryEncoding<int> a, BinaryEncoding<int> b)
{
    auto a_data = a.data();
    auto b_data = b.data();

    return (~(a_data ^ b_data)).count();
}

int main()
{
    GeneticAlgorithm<BinaryEncoding<int>> ga (
        100,
        BinaryEncoding<int>::birth,
        [](BinaryEncoding<int>& bin){return fitness(bin, BinaryEncoding<int>(1000)); },
        BinaryEncoding<int>::mutate<20>,
        BinaryEncoding<int>::crossover,
        .1f
    );

    print_pop<BinaryEncoding<int>>(ga, 5, [](BinaryEncoding<int> n){ return std::to_string(n.get()); });
    ga.evolve_until_fitness(32.f);
    print_pop<BinaryEncoding<int>>(ga, 5, [](BinaryEncoding<int> n){ return std::to_string(n.get()); });
}