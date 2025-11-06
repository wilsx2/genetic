#ifndef GA_H
#define GA_H

#include "population.h"

#include <vector>
#include <functional>
#include <string>

template <typename T>
class GeneticAlgorithm {
    private:
        Population<T> population_;
        
        const std::function<float(T&)> fitness_;
        const std::function<void(T&)> mutate_;
        const std::function<T(T&,T&)> crossover_;
        const float elitism_rate_;     
        
    public:
        GeneticAlgorithm(
            std::size_t population_size,
            std::function<T()> birth,
            std::function<float(T&)> fitness,
            std::function<void(T&)> mutate,
            std::function<T(T&,T&)> crossover,
            float elitism_rate
        );
        void evolve();
        void evolve(std::size_t n);
        void evolve_until_fitness(float target);
        const Population<T>& getPopulation() const;

        bool save_population(std::string filepath);
        bool load_population(std::string filepath);
};

#include "ga.tpp"
#endif