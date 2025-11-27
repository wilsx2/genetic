#ifndef GA_H
#define GA_H

#include "member.h"
#include "population_history.h"
#include "operator/selection.h"
#include "serialization/serializer.h"

#include <optional>
#include <vector>
#include <functional>
#include <string>
#include <filesystem>
#include "utils/rng.h"

namespace genetic 
{

template <typename T>
class GeneticAlgorithm
{
    private:
        using FitnessFunction = std::function<float(const T&)>;
        using BirthFunction = std::function<T(util::RNG&)>;
        using MutationOperator = std::function<void(T&, util::RNG&)>;
        using CrossoverOperator = std::function<T(const T&, const T&, util::RNG&)>;

        const std::string problem_;
        PopulationHistory<T> population_;
        
        const FitnessFunction fitness_function_;
        const BirthFunction birth_function_;
        const MutationOperator mutate_function_;
        const CrossoverOperator crossover_function_;
        const float elitism_rate_;     

        selection::Function<T> selection_function_;

        Serializer<T> serializer_;
        util::RNG rng_;

        inline std::size_t numElites();
        
    public:
        struct Config
        {
            std::string problem;
            FitnessFunction fitness;
            BirthFunction birth;
            MutationOperator mutate;
            CrossoverOperator crossover;
            selection::Function<T> select = selection::tournament<int, 5>;
            std::size_t population_size = 1000;
            float elitism_rate = .1f;
        };

        GeneticAlgorithm(const Config& config);
        void restart();
        void evolve();
        
        const std::string& getProblem() const;
        const PopulationHistory<T>& getPopulation() const;

        // Expose serializer functionality
        bool savePopulation();
        bool loadPopulation(std::string id);
        std::vector<std::string> getSaves() const;
        bool deleteSave(const std::string& id) const;
        bool deleteAllSaves() const;
};

}

#include "ga.tpp"
#endif