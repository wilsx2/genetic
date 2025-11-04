#include "ga.h"
#include <random>

template <typename T>
Genetic<T>::Genetic(
    std::size_t population_size,
    std::function<T()> birth,
    std::function<float(T&)> fitness,
    std::function<void(T&)> mutate,
    std::function<void(T&,T&)> crossover,
    float mutation_rate,
    float elitism_rate
)   : fitness_(fitness)
    , mutate_(mutate)
    , crossover_(crossover)
    , mutation_rate_(mutation_rate)
    , elitism_rate_(elitism_rate)
{
    for (int i = 0; i < population_size; ++i)
    {
        population_.emplace_back(birth());
    }
    fitness_scores_.resize(population_size);
}

template <typename T>
void Genetic<T>::evolve()
{
    // Evaluate fitness
    total_fitness_ = 0.f;
    for(int i = 0; i < population_.size(); ++i)
    {
        float fitness_score = fitness_(population_[i])
        fitness_scores_[i] = fitness_score;
        total_fitness_ += fitness_score;
    }

    // Create new population
    std::vector<T> new_population;

    /// Apply Elitism
    // TOOD
    
    /// Populate
    std::function<T&()> selection = [&](){
        std::uniform_real_distribution<float> wheel(0.f, total_fitness_);
        float spin = wheel();
        int i = 0;
        while(spin > fitness_scores_[i])
        {
            spin -= fitness_scores_[i];
            ++i;
        }
        return population_[i];
    };
    while (new_population.size() < population_.size())
    {
        // Select
        T& parent_a = selection();
        T& parent_b = selection();

        // Crossover
        T offspring = crossover_(parent_a, parent_b);

        // Mutate
        std::uniform_real_distribution<float> chance(0.f, 1.f);
        if(chance(gen_) < mutation_rate_)
            mutate_(offspring);
        new_population.push_back(offspring);
    }

    population_ = new_population;
}