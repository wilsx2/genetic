#include "population.h"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <format>
#include <typeinfo>
#include <cassert>

template <typename T>
Population<T>::Population()
    : identifier(rand())
    , generation(1)
    , total_fitness(0.f) 
{}

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
void Population<T>::add(T&& new_member)
{
    members.emplace_back(new_member, 0.f);
}

template <typename T>
void Population<T>::newGeneration(std::vector<T>& new_gen)
{
    int start = members.size() - new_gen.size();
    assert(start >= 0);

    // Overwrites least-fit members with new generation
    for(int i = start; i < members.size(); ++i)
    {
        members[i].value = new_gen[i - start];
    }

    ++generation;
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
bool Population<T>::save(std::string label)
{

    std::ofstream output (
        "populations/" + label
        + "_P" + std::format("{:x}", identifier)
        + "_G" + std::to_string(generation)
        + "_F" + std::to_string(fittest().fitness)
    );
    
    if (!output.is_open())
        return false;

    // Tag With Type ID
    std::size_t typeHash = typeid(T).hash_code();
    output.write(reinterpret_cast<char*>(&typeHash), sizeof(std::size_t));

    // Identifier
    output.write(reinterpret_cast<char*>(&identifier), sizeof(u_int32_t));
    
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
bool Population<T>::load(std::string filename)
{
    std::ifstream input ("populations/"+filename);
    
    if (!input.is_open())
        return false;
    
    // Check Type ID tag
    std::size_t inputTypeHash;
    input.read(reinterpret_cast<char*>(&inputTypeHash), sizeof(std::size_t));
    if (inputTypeHash != typeid(T).hash_code())
        return false;

    // Identifier
    input.read(reinterpret_cast<char*>(&identifier), sizeof(u_int32_t));

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