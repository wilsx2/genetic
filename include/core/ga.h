#ifndef GA_H
#define GA_H

#include "scenario.h"
#include "member.h"
#include "population_history.h"
#include "operator/selection.h"
#include "serialization/serializer.h"
#include "utils/rng.h"

#include <optional>
#include <vector>
#include <functional>
#include <string>
#include <filesystem>
#include <type_traits>
#include <memory>

namespace genetic 
{

template <typename T>
class GeneticAlgorithm
{
    private:
        std::unique_ptr<Scenario<T>> scenario_;

        const float elitism_rate_;

        selection::Function<T> selection_function_;

        PopulationHistory<T> population_;

        util::RNG rng_;

        inline std::size_t numElites();
        
    public:
        GeneticAlgorithm(
            std::unique_ptr<Scenario<T>> scenario,
            selection::Function<T> select,
            std::size_t population_size,
            float elitism_rate
        );
        GeneticAlgorithm(
            std::unique_ptr<Scenario<T>> scenario
        );
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