#include "population.h"

#include <algorithm>
#include <iostream>
#include <fstream>

template <typename T>
Population<T>::Population(): generation(1), total_fitness(0.f) {}

template <typename T>
Member<T>& Population<T>::operator[](std::size_t idx)
{
    return members[idx];
}

template <typename T>
const Member<T>& Population<T>::operator[](std::size_t idx) const
{
    return members[idx];
}

template <typename T>
Member<T>& Population<T>::fittest()
{
    return members[0];
}

template <typename T>
const Member<T>& Population<T>::fittest() const
{
    return members[0];
}

template <typename T>
std::size_t Population<T>::size() const
{
    return members.size();
}

template <typename T>
void Population<T>::sort()
{
    std::sort(members.begin(), members.end(),
    [](const Member<T>& a, const Member<T>& b) {
        return a.fitness > b.fitness;
    });
}

template <typename T>
bool Population<T>::save(std::string filepath)
{
    /*
        NOTE: In the future should not take filepath as a parameter. Should instead output with the format of:
            Description of data in plaintext, retrieved from source type ideally.
            Identifier (hash) of the population, generated when first created and stored in GAs using that population
            Current generation of that population
    */

    std::ofstream output (filepath);
    
    if (!output.is_open())
        return false;
    
    // Current Generation
    output.write(reinterpret_cast<char*>(&generation), sizeof(std::size_t));

    // Size
    auto size = members.size();
    output.write(reinterpret_cast<char*>(&size), sizeof(std::size_t));

    // Members
    output.write(reinterpret_cast<char*>(members.data()), sizeof(T)*size);

    output.close();
    return true;
}

template <typename T>
bool Population<T>::load(std::string filepath)
{
    std::ifstream input (filepath);
    
    if (!input.is_open())
        return false;
    // TODO: Validate that save file is of the same type

    // Current Generation
    input.read(reinterpret_cast<char*>(&generation), sizeof(std::size_t)); 

    // Size
    std::size_t size;
    input.read(reinterpret_cast<char*>(&size), sizeof(std::size_t)); 

    // Members
    members.resize(size);
    input.read(reinterpret_cast<char*>(members.data()), sizeof(T)*size); 

    input.close();
    return true;
}