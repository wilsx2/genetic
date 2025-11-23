#include "ga.h"
#include <algorithm>
#include <cassert>
#include <ctime>

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
    , birth_function_(std::move(birth))
    , fitness_function_(std::move(fitness))
    , mutate_function_(std::move(mutate))
    , crossover_function_(std::move(crossover))
    , selection_function_(std::move(select))
    , elitism_rate_(elitism_rate)
    , serializer_(problem_)
{
    if (!(elitism_rate >= 0.f && elitism_rate <= 1.f))
        throw std::invalid_argument("elitism_rate must be in the interval [0, 1]");
    if (problem_.size() == 0)
        throw std::invalid_argument("problem name must be non-empty");

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
    std::sort(population_.begin(), population_.end());
    fittest_of_each_generation_.push_back(population_.back());
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


template <typename T>
bool GeneticAlgorithm<T>::savePopulation()
{
    PopulationData<T> data;
    data.id = population_identifier_;
    data.current_population = population_;
    data.fittest_history = fittest_of_each_generation_;
    
    return serializer_.save(data, getGeneration(), getFittestScore());
}

template <typename T>
bool GeneticAlgorithm<T>::loadPopulation(std::string id)
{
    std::optional<PopulationData<T>> data = serializer_.load(id);

    if (data.has_value())
    {
        population_identifier_ = std::move(data->id);
        population_ = std::move(data->current_population);
        fittest_of_each_generation_ = std::move(data->fittest_history);
        return true;
    }
    return false;
}

template <typename T>
std::vector<std::string> GeneticAlgorithm<T>::getSaves() const
{
    return serializer_.getSaves();
}

template <typename T>
bool GeneticAlgorithm<T>::deleteSave(const std::string& id) const
{
    return serializer_.deleteSave(id);
}

template <typename T>
bool GeneticAlgorithm<T>::deleteAllSaves() const
{
    return serializer_.deleteAllSaves();
}