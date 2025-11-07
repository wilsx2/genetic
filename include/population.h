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
    
    u_int32_t identifier;
    std::vector<Member<T>> members;
    std::vector<Member<T>> best_of;
    std::size_t generation;
    float total_fitness;

    Member<T>& operator[](std::size_t idx);
    const Member<T>& operator[](std::size_t idx) const;
    Member<T>& fittest();
    const Member<T>& fittest() const;
    std::size_t size() const;
    void add(T&& new_member);
    void newGeneration(std::vector<T>& new_gen);
    void sort();
    bool save(std::string label);
    bool load(std::string filename);
};

#include "population.tpp"
#endif