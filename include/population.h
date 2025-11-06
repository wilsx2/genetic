#ifndef POPULATION_H
#define POPULATION_H

#include <vector>
#include <string>

template <typename T>
struct Member
{
    T value;
    float fitness;
};

template <typename T>
struct Population
{
    static_assert(std::is_trivially_copyable_v<T> == true);

    Population();
    
    std::vector<Member<T>> members;
    std::size_t generation;
    float total_fitness;

    Member<T>& operator[](std::size_t idx);
    const Member<T>& operator[](std::size_t idx) const;
    Member<T>& fittest();
    const Member<T>& fittest() const;
    std::size_t size() const;
    void sort();
    bool save(std::string filepath);
    bool load(std::string filepath);
};

#include "population.tpp"
#endif