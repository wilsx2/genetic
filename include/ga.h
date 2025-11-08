#ifndef GA_H
#define GA_H

#include "member.h"
#include "selection.h"

#include <vector>
#include <functional>
#include <string>

template <typename T>
class GeneticAlgorithm {
    static_assert(std::is_trivially_copyable_v<T> == true);

    private:
        std::vector<Member<T>> population_;
        u_int32_t population_identifier_;
        std::vector<Member<T>> best_of_each_generation_;
        
        const std::function<float(T&)> fitness_function_;
        const std::function<void(T&)> mutate_function_;
        const std::function<T(T&,T&)> crossover_function_;
        const float elitism_rate_;     

        selection::Func<T> selection_function_;
        
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
        const std::vector<Member<T>>& getPopulation() const;
        const std::vector<Member<T>>& getBestOfEachGeneration() const;
        std::size_t getGeneration() const;

        bool savePopulation(std::string label);
        bool loadPopulation(std::string filename);
};

#include "ga.tpp"
#endif