#include "ga.h"
#include <algorithm>
#include <cassert>
#include <ctime>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <format>

template <typename T>
GeneticAlgorithm<T>::GeneticAlgorithm(
    std::size_t population_size,
    std::function<T()> birth,
    std::function<float(T&)> fitness,
    std::function<void(T&)> mutate,
    std::function<T(T&,T&)> crossover,
    float elitism_rate,
    selection::Func<T> select
)   : fitness_function_(fitness)
    , mutate_function_(mutate)
    , crossover_function_(crossover)
    , elitism_rate_(elitism_rate)
    , selection_function_(select)
{
    population_.reserve(population_size);
    for (int i = 0; i < population_size; ++i)
    {
        T member = birth();
        float fitness_score = fitness_function_(member);
        population_.emplace_back(std::move(member), fitness_score);
    }

    std::sort(population_.begin(), population_.end(),
    [](const Member<T>& a, const Member<T>& b) {
        return a.fitness > b.fitness;
    });
    best_of_each_generation_.push_back(population_[0]);
}

template <typename T>
inline std::size_t GeneticAlgorithm<T>::numElites()
{
    return population_.size() * elitism_rate_;
}

template <typename T>
void GeneticAlgorithm<T>::evolve()
{
    std::vector<Member<T>> parents = population_;
    
    // Replace non-elites with children
    for (int i = numElites(); i < population_.size(); ++i)
    {
        // Select
        T& parent_a = selection_function_(parents);
        T& parent_b = selection_function_(parents);

        // Crossover
        T offspring = crossover_function_(parent_a, parent_b);

        // Mutate
        mutate_function_(offspring);
        
        /// Overwrite old member
        population_[i].fitness = fitness_function_(offspring);
        population_[i].value = std::move(offspring);
    }

    std::sort(population_.begin(), population_.end(),
    [](const Member<T>& a, const Member<T>& b) {
        return a.fitness > b.fitness;
    });
    best_of_each_generation_.push_back(population_[0]);
}

template <typename T>
void GeneticAlgorithm<T>::evolve(std::size_t n)
{
    for (int i = 0; i < n; ++i) evolve();
}

template <typename T>
void GeneticAlgorithm<T>::evolveUntilFitness(float target)
{
    while(population_[0].fitness < target) evolve();
}

template <typename T>
const std::vector<Member<T>>& GeneticAlgorithm<T>::getPopulation() const
{
    return population_;
}

template <typename T>
const std::vector<Member<T>>& GeneticAlgorithm<T>::getBestOfEachGeneration() const
{
    return best_of_each_generation_;
}

template <typename T>
std::size_t GeneticAlgorithm<T>::getGeneration() const
{
    return best_of_each_generation_.size();
}

template <typename T>
bool GeneticAlgorithm<T>::savePopulation(std::string label)
{
    std::ofstream output (
        "populations/" + label
        + "_P" + std::format("{:x}", population_identifier_)
        + "_G" + std::to_string(best_of_each_generation_.size())
        + "_F" + std::to_string(population_[0].fitness)
    );
    
    if (!output.is_open())
        return false;

    // Tag With Type ID
    std::size_t typeHash = typeid(T).hash_code();
    output.write(reinterpret_cast<char*>(&typeHash), sizeof(std::size_t));

    // Identifier
    output.write(reinterpret_cast<char*>(&population_identifier_), sizeof(u_int32_t));

    // Best Of
    /// Size
    std::size_t size = best_of_each_generation_.size();
    output.write(reinterpret_cast<char*>(&size), sizeof(std::size_t));

    /// Members
    output.write(reinterpret_cast<char*>(best_of_each_generation_.data()), sizeof(Member<T>)*size);
    
    // Current Generation
    /// Size
    size = population_.size();
    output.write(reinterpret_cast<char*>(&size), sizeof(std::size_t));

    /// Members
    output.write(reinterpret_cast<char*>(population_.data()), sizeof(Member<T>)*size);

    output.close();
    return true;
}

template <typename T>
bool GeneticAlgorithm<T>::loadPopulation(std::string filename)
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
    input.read(reinterpret_cast<char*>(&population_identifier_), sizeof(u_int32_t));

    // Best Of
    /// Size
    std::size_t size;
    input.read(reinterpret_cast<char*>(&size), sizeof(std::size_t));

    /// Members
    best_of_each_generation_.resize(size);
    input.read(reinterpret_cast<char*>(best_of_each_generation_.data()), sizeof(Member<T>)*size); 

    // Current Generation
    /// Size
    input.read(reinterpret_cast<char*>(&size), sizeof(std::size_t)); 

    /// Members
    population_.resize(size);
    input.read(reinterpret_cast<char*>(population_.data()), sizeof(Member<T>)*size); 

    input.close();
    return true;
}