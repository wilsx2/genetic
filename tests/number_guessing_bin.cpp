#include "ga.h"
#include "binary_encoding.h"
#include <string>
#include <cstdlib>
#include <climits>
#include <iostream>

void print_pop(Genetic<BinaryEncoding<int>>& ga)
{
    std::cout << "Generation " << ga.getGeneration() << "\n";
    const auto& pop = ga.getPopulation();
    for(int i = 0; i < 10 && i < pop.size(); ++i)
    {
        auto& pair = pop[i];
        std::cout << (pair.first.data().to_ulong()) << " | " << pair.second << "\n";
    }
}

float fitness(BinaryEncoding<int> a, BinaryEncoding<int> b)
{
    auto a_data = a.data();
    auto b_data = b.data();

    return (~(a_data ^ b_data)).count();
}

int main()
{
    Genetic<BinaryEncoding<int>> ga (
        100,
        BinaryEncoding<int>::birth,
        [](BinaryEncoding<int>& bin){return fitness(bin, BinaryEncoding<int>(1000)); },
        BinaryEncoding<int>::mutate<20>,
        BinaryEncoding<int>::crossover,
        .1f
    );

    print_pop(ga);
    ga.evolve_until_fitness(32.f);
    print_pop(ga);
}