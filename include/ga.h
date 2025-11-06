#ifndef GA_H
#define GA_H

#include "population.h"
#include "selection.h"

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

        selection::Func<T> select_;
        
    public:
        GeneticAlgorithm(
            std::size_t population_size,
            std::function<T()> birth,
            std::function<float(T&)> fitness,
            std::function<void(T&)> mutate,
            std::function<T(T&,T&)> crossover,
            float elitism_rate,
            selection::Func<T> select
        );
        inline std::size_t numElites();
        void evaluateFitness();
        void evolve();
        void evolve(std::size_t n);
        void evolveUntilFitness(float target);
        const Population<T>& getPopulation() const;

        bool savePopulation(std::string label);
        bool loadPopulation(std::string filename);
};

#include "ga.tpp"
#endif