#ifndef GA_H
#define GA_H

#include <vector>
#include <functional>

template <typename T>
class Genetic {
    private:
        std::random_device rd_;
        std::mt19937 gen_ = std::mt19937(rd());

        std::vector<T> population_;
        std::vector<float> fitness_scores_;
        float total_fitness_;
        std::size_t generation_;
        
        const std::function<float(T&)> fitness_;
        const std::function<void(T&)> mutate_;
        const std::function<void(T&,T&)> crossover_;

        const float mutation_rate_;
        const float elitism_rate_;      
        
    public:
        Genetic(
            std::size_t population_size,
            std::function<T()> birth,
            std::function<float(T&)> fitness,
            std::function<void(T&)> mutate,
            std::function<void(T&,T&)> crossover,
            float mutation_rate,
            float elitism_rate
        );
        void evolve();
};

#endif