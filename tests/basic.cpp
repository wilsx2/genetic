#include "ga.h"
#include <cstdlib>
#include <climits>
#include <iostream>

int main()
{
    Genetic<int> ga (
        1000,
        [](){return rand();},
        [](int& n){return -abs(1000 - n); },
        [](int& n){return n += (1 - 2*(rand()%2)) * 10000; },
        [](int& a, int& b){return a/2 + b/2; },
        .2f,
        .1
    );
    for(auto& pair : ga.getPopulation())
    {
        std::cout << pair.first << " | " << pair.second << "\n";
    }
    std::cout << " evolve";
    for(int i = 0; i < 1000; ++i)
        ga.evolve();
    for(auto& pair : ga.getPopulation())
    {
        std::cout << pair.first << " | " << pair.second << "\n";
    }
}
