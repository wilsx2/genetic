#ifndef GA_H
#define GA_H

#include <vector>
#include <functional>

template <typename T>
class Genetic {
    private:
        std::vector<T> population_;
        std::vector<float> fitness_scores_;
        float total_fitness_;
        std::size_t generation_;
        
        const std::function<float(T&)> fitness_;
        const std::function<void(T&)> mutate_;
        const std::function<void(T&,T&)> crossover_;

        const float mutation_rate_;
        const float crossover_rate_;
        const float elitism_rate_;      
        
    public:
        Genetic(
            std::size_t population_size,
            std::function<float(T&)> fitness,
            std::function<void(T&)> mutate,
            std::function<void(T&,T&)> crossover,
            float mutation_rate,
            float crossover_rate,
            float elitism_rate
        );
        evolve();
};

#endif