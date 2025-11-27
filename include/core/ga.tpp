#include "ga.h"
#include <cassert>
#include <ctime>

namespace genetic 
{

template <typename T>
GeneticAlgorithm<T>::GeneticAlgorithm(
    std::string problem,
    FitnessFunction fitness,
    BirthFunction birth,
    MutationOperator mutate,
    CrossoverOperator crossover,
    selection::Function<T> select,
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
    , rng_()
    , population_(0, population_size)
{
    if (!(elitism_rate >= 0.f && elitism_rate <= 1.f))
        throw std::invalid_argument("elitism_rate must be in the interval [0, 1]");
    if (problem_.size() == 0)
        throw std::invalid_argument("problem name must be non-empty");

    restart();
}

template <typename T>
void GeneticAlgorithm<T>::restart()
{
    std::size_t size = population_.populationSize();
    population_.restart(rng_.index(UINT32_MAX), size);
    
    std::vector<Member<T>> next;
    next.reserve(size);
    while (next.size() < size)
    {
        T member = birth_function_(rng_);
        next.emplace_back(fitness_function_(member), std::move(member));
    }

    population_.pushNext(std::move(next));
}

template <typename T>
void GeneticAlgorithm<T>::evolve()
{
    const Generation<T>& parents = population_.current();

    std::vector<Member<T>> next;
    next.reserve(population_.populationSize());

    // Elitism
    for (int i = 0; i < numElites(); ++i)
    {
        next.push_back(parents[parents.size() - i - 1]);
    }

    // Mutation & Crossover
    while (next.size() < population_.populationSize())
    {
        // Select
        const T& parent_a = selection_function_(parents, rng_);
        const T& parent_b = selection_function_(parents, rng_);

        // Crossover
        T offspring = crossover_function_(parent_a, parent_b, rng_);

        // Mutate
        mutate_function_(offspring, rng_);
        
        /// Add to new generation
        next.emplace_back(fitness_function_(offspring), std::move(offspring));
    }

    // Finalize
    population_.pushNext(std::move(next));
}

template <typename T>
inline std::size_t GeneticAlgorithm<T>::numElites()
{
    return population_.populationSize() * elitism_rate_;
}

template <typename T>
const std::string& GeneticAlgorithm<T>::getProblem() const
{
    return problem_;
}

template <typename T>
const PopulationHistory<T>& GeneticAlgorithm<T>::getPopulation() const
{
    return population_;
}

template <typename T>
bool GeneticAlgorithm<T>::savePopulation()
{
    return serializer_.save(population_);
}

template <typename T>
bool GeneticAlgorithm<T>::loadPopulation(std::string id)
{
    std::optional<PopulationHistory<T>> data = serializer_.load(id);

    if (data.has_value())
    { 
        population_ = data.value();
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

}