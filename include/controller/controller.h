#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "command_handler.h"
#include "view.h"
#include "core/ga.h"
#include <memory>
#include <string>
#include <functional>
#include <variant>
#include <map>

namespace genetic 
{

template <typename T>
using ViewCallback = std::function<void(const std::vector<Member<T>>&, ViewType)>;

template <typename T>
class Controller
{
    private:
        using EvolutionCondition = std::function<bool(const PopulationHistory<T>& pop, float time)>;

        GeneticAlgorithm<T> ga_;
        util::CommandHandler command_handler_;
        std::unique_ptr<View<T>> view_;
        bool running_;
    
    public:
        // Lifecycle
        Controller(GeneticAlgorithm<T>&& ga, std::unique_ptr<View<T>> view);
        void run();
        void stop();

        // Save management
        void restart();
        void save();
        void load(const std::string& id);
        void listSaves();
        void deleteSave(const std::string& id);
        void deleteAllSaves();

        // Viewing
        void printStats();
        void viewGeneration(std::size_t i);
        void viewCurrent();
        void viewBest();

        // Evolution
        void evolve(EvolutionCondition condition);
        void evolveGenerations(int generations);
        void evolveSeconds(float seconds);
        void evolveUntilFitness(float target_fitness);
        void evolveUntilGeneration(int target_generation);
        void evolveUntilStagnant(int generational_average, float minimum_improvement);
};

}

#include "controller.tpp"
#endif