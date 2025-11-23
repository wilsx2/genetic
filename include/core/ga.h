#ifndef GA_H
#define GA_H

#include "member.h"
#include "operator/selection.h"
#include "serialization/serializer.h"

#include <optional>
#include <vector>
#include <functional>
#include <string>
#include <filesystem>
#include "utils/rng.h"

template <typename T>
class GeneticAlgorithm {
    private:
        using FitnessFunction = std::function<float(T&)>;
        using BirthFunction = std::function<T(RNG&)>;
        using MutationOperator = std::function<void(T&, RNG&)>;
        using CrossoverOperator = std::function<T(T&,T&, RNG&)>;

        const std::string problem_;
        std::vector<Member<T>> population_;
        uint32_t population_identifier_;
        std::vector<Member<T>> fittest_of_each_generation_;
        
        const FitnessFunction fitness_function_;
        const BirthFunction birth_function_;
        const MutationOperator mutate_function_;
        const CrossoverOperator crossover_function_;
        const float elitism_rate_;     

        selection::Func<T> selection_function_;

        Serializer<T> serializer_;
        RNG rng_;

        inline std::size_t numElites();
        
    public:
        GeneticAlgorithm(
            std::string problem,
            FitnessFunction fitness,
            BirthFunction birth,
            MutationOperator mutate,
            CrossoverOperator crossover,
            selection::Func<T> select,
            std::size_t population_size,
            float elitism_rate
        );
        void newPopulation(std::size_t size);
        void rankAndRecordFittest();
        void evolve();
        
        const std::vector<Member<T>>& getPopulation() const;
        const std::vector<Member<T>>& getFittestOfEachGeneration() const;
        float getFittestScore() const;
        std::size_t getGeneration() const;
        std::string getFormattedId() const;
        const std::string& getProblem() const;

        bool savePopulation();
        bool loadPopulation(std::string id);
};

#include "ga.tpp"
#endif