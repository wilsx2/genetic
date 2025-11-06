#include "test_util.hpp"
#include "ga.h"

#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[])
{
    if(argc != 2){
        std::cerr << "File name and only one file name must be provided from populations/\n";
        return 1;
    }

    GeneticAlgorithm<int> ga (
        10,
        [](){return rand();},
        [](int&){return 0.f; },
        [](int&){return;},
        [](int&, int&){return rand();},
        .1f,
        selection::tournament<int, 5>
    );

    std::string filename (argv[1]);
    bool success = ga.loadPopulation(filename);
    if (!success) {
        std::cerr << "File name \"" << filename << "\" could not be opened.";
        return 1;
    }
    printPopulation<int>(ga, 5, [](int n){ return std::to_string(n); });
}