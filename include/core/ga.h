#ifndef GA_H
#define GA_H

#include "member.h"
#include "operator/selection.h"

#include <optional>
#include <vector>
#include <functional>
#include <string>
#include <filesystem>

template <typename T>
class GeneticAlgorithm {
    static_assert(std::is_trivially_copyable_v<T> == true);

    private:
        const std::string problem_;
        const std::string save_directory_;
        std::vector<Member<T>> population_;
        uint32_t population_identifier_;
        std::vector<Member<T>> fittest_of_each_generation_;
        
        const std::function<T()> birth_function_;
        const std::function<float(T&)> fitness_function_;
        const std::function<void(T&)> mutate_function_;
        const std::function<T(T&,T&)> crossover_function_;
        const float elitism_rate_;     

        selection::Func<T> selection_function_;

        inline std::size_t numElites();
        std::optional<std::filesystem::path> findPopulationFile(std::string id);
        
    public:
        GeneticAlgorithm(
            std::string problem,
            std::size_t population_size,
            std::function<T()> birth,
            std::function<float(T&)> fitness,
            std::function<void(T&)> mutate,
            std::function<T(T&,T&)> crossover,
            float elitism_rate,
            selection::Func<T> select
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