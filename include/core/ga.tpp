#include "ga.h"
#include <algorithm>
#include <cassert>
#include <ctime>
#include <filesystem>
#include <format>
#include <iostream>
#include <fstream>


template <typename T>
GeneticAlgorithm<T>::GeneticAlgorithm(
    std::string problem,
    FitnessFunction fitness,
    BirthFunction birth,
    MutationOperator mutate,
    CrossoverOperator crossover,
    selection::Func<T> select,
    std::size_t population_size,
    float elitism_rate
)   : problem_(std::move(problem))
    , save_directory_("populations/"+problem+"/")
    , birth_function_(std::move(birth))
    , fitness_function_(std::move(fitness))
    , mutate_function_(std::move(mutate))
    , crossover_function_(std::move(crossover))
    , selection_function_(std::move(select))
    , elitism_rate_(elitism_rate)
{
    if (!(elitism_rate >= 0.f && elitism_rate <= 1.f))
        throw std::invalid_argument("elitism_rate must be in the interval [0, 1]")

    newPopulation(population_size);
}

template <typename T>
void GeneticAlgorithm<T>::newPopulation(std::size_t size)
{
    if (size == 0)
        throw std::invalid_argument("Population size must be greater than 0");

    if (population_.size() != 0 || fittest_of_each_generation_.size() != 0)
    {
        fittest_of_each_generation_.clear();
        population_.clear();
    }

    population_identifier_ = rng_.index(UINT32_MAX);
    population_.reserve(size);
    while (population_.size() < size)
    {
        T member = birth_function_(rng_);
        float fitness_score = fitness_function_(member);
        population_.emplace_back(std::move(member), fitness_score);
    }

    rankAndRecordFittest();   
}

template <typename T>
void GeneticAlgorithm<T>::evolve()
{
    std::vector<Member<T>> parents = population_;
    
    // Replace non-elites with children
    for (int i = 0; i < population_.size() - numElites(); ++i)
    {
        // Select
        T& parent_a = selection_function_(parents, rng_);
        T& parent_b = selection_function_(parents, rng_);

        // Crossover
        T offspring = crossover_function_(parent_a, parent_b, rng_);

        // Mutate
        mutate_function_(offspring, rng_);
        
        /// Overwrite old member
        population_[i].fitness = fitness_function_(offspring);
        population_[i].value = std::move(offspring);
    }

    rankAndRecordFittest();
}

template <typename T>
void GeneticAlgorithm<T>::rankAndRecordFittest()
{
    std::sort(population_.begin(), population_.end(),
    [](const Member<T>& a, const Member<T>& b) {
        return a.fitness < b.fitness;
    });
    fittest_of_each_generation_.push_back(population_.back());
}

template <typename T>
std::optional<std::filesystem::path> GeneticAlgorithm<T>::findPopulationFile(std::string id)
{
    if (!std::filesystem::exists(save_directory_) || !std::filesystem::is_directory(save_directory_))
    {
        return std::nullopt;
    }

    for (const auto& entry : std::filesystem::directory_iterator(save_directory_))
    {
        if (!entry.is_regular_file())
            continue;

        const auto filename = entry.path().filename().string();
        if (filename.rfind(id, 0) == 0)
        {
            return entry.path();
        }
    }

    return std::nullopt;
}

template <typename T>
bool GeneticAlgorithm<T>::savePopulation()
{
    // Ensure there's a place for our save
    std::filesystem::create_directories(save_directory_);

    // Overwrite previous save of this population, if it exists
    std::optional<std::filesystem::path> path = findPopulationFile(getFormattedId());
    if (path.has_value())
        std::filesystem::remove(path.value());

    // Begin saving
    std::ofstream output (
        save_directory_
        + getFormattedId()
        + "_G" + std::to_string(getGeneration())
        + "_F" + std::to_string(getFittestScore())
    );
    
    if (!output.is_open()) {
        std::cerr << "Failed to create save" << std::flush;
        return false;
    }

    // Tag With Type ID
    std::size_t typeHash = typeid(T).hash_code();
    output.write(reinterpret_cast<char*>(&typeHash), sizeof(std::size_t));

    // Identifier
    output.write(reinterpret_cast<char*>(&population_identifier_), sizeof(uint32_t));

    // Fittest
    /// Size
    std::size_t size = fittest_of_each_generation_.size();
    output.write(reinterpret_cast<char*>(&size), sizeof(std::size_t));
    /// Members
    output.write(reinterpret_cast<char*>(fittest_of_each_generation_.data()), sizeof(Member<T>)*size);
    
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
bool GeneticAlgorithm<T>::loadPopulation(std::string id)
{
    // Search for file with id provided
    std::optional<std::filesystem::path> path = findPopulationFile(id);
    if (!path.has_value()) {
        std::cerr << "No file in the \"" << save_directory_ <<
        "\" directory matches the prefix \"" << id << "\"\n";
        return false;
    }

    // Begin loading
    std::ifstream input (path.value().string());
    if (!input.is_open()) {
        std::cerr << "Failed to open file \"" << path.value().string() << "\"\n";
        return false;
    }
    
    // Check Type ID tag
    std::size_t inputTypeHash;
    input.read(reinterpret_cast<char*>(&inputTypeHash), sizeof(std::size_t));
    if (inputTypeHash != typeid(T).hash_code()) {
        std::cerr << "File \"" << path.value().string()
            << "\" stores a population of a type other than \""
            << typeid(T).name() << "\"\n";
        return false;
    }

    // Identifier
    input.read(reinterpret_cast<char*>(&population_identifier_), sizeof(u_int32_t));

    // Fittest
    /// Size
    std::size_t size;
    input.read(reinterpret_cast<char*>(&size), sizeof(std::size_t));
    /// Members
    fittest_of_each_generation_.resize(size);
    input.read(reinterpret_cast<char*>(fittest_of_each_generation_.data()), sizeof(Member<T>)*size); 

    // Current Generation
    /// Size
    input.read(reinterpret_cast<char*>(&size), sizeof(std::size_t)); 
    /// Members
    population_.resize(size);
    input.read(reinterpret_cast<char*>(population_.data()), sizeof(Member<T>)*size); 

    input.close();
    return true;
}

template <typename T>
inline std::size_t GeneticAlgorithm<T>::numElites()
{
    return population_.size() * elitism_rate_;
}

template <typename T>
const std::vector<Member<T>>& GeneticAlgorithm<T>::getPopulation() const
{
    return population_;
}

template <typename T>
const std::vector<Member<T>>& GeneticAlgorithm<T>::getFittestOfEachGeneration() const
{
    return fittest_of_each_generation_;
}
template <typename T>
float GeneticAlgorithm<T>::getFittestScore() const
{
    if (population_.size() == 0)
        throw std::logic_error("Cannot get fittest of empty population");
        
    return population_.back().fitness;
}

template <typename T>
std::size_t GeneticAlgorithm<T>::getGeneration() const
{
    return fittest_of_each_generation_.size();
}

template <typename T>
std::string GeneticAlgorithm<T>::getFormattedId() const
{
    return std::format("{:x}", population_identifier_);
}

template <typename T>
const std::string& GeneticAlgorithm<T>::getProblem() const
{
    return problem_;
}