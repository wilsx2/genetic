#ifndef GA_H
#define GA_H

#include <vector>
#include <functional>
#include <random>

template <typename T>
class Genetic {
    private:
        std::mt19937 gen_;

        std::vector<std::pair<T, float>> population_;
        std::size_t generation_;
        
        const std::function<float(T&)> fitness_;
        const std::function<void(T&)> mutate_;
        const std::function<T(T&,T&)> crossover_;
        const float elitism_rate_;     
        
        template<std::size_t N> T& tournamentSelect();
        void sortPopulation();
        
    public:
        Genetic(
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
        const std::vector<std::pair<T,float>>& getPopulation();
        std::size_t getGeneration();
};

#include "ga.tpp"
#endif