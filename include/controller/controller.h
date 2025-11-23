#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "core/ga.h"
#include <string>
#include <functional>
#include <variant>
#include <map>

enum class ViewType {Generations, Population};
template<typename T>
class Controller
{
    private:
        using ArgumentList = std::vector<std::string>&;
        using CommandCallback = std::function<void(ArgumentList)>;
        using ViewCallback = std::function<void(const std::vector<Member<T>>&, ViewType)>;
        using EvolutionCondition = std::function<bool(const GeneticAlgorithm<T>& ga, float time)>;

        GeneticAlgorithm<T> ga_;
        std::map<std::string, CommandCallback> commands_;
        const ViewCallback view_function_;
        bool running_;

        template <typename V>
        static V fromString(const std::string& s);
        template<auto MemberFunc>
        CommandCallback bindCommand();
        void executeCommand(const std::string& input);
    
    public:
        Controller(GeneticAlgorithm<T>&& ga, ViewCallback view);
        void run();

        void restart();
        void save();
        void load(std::string id);
        void listSaves();
        void deleteSave(std::string id);
        void deleteAllSaves();

        void printStats();
        void viewPopulation();
        void viewBest();

        void evolve(EvolutionCondition break_condition);
        void evolveGenerations(int generations);
        void evolveSeconds(float seconds);
        void evolveUntilFitness(float target_fitness);
        void evolveUntilGeneration(int target_generation);
        void evolveUntilStagnant(int generational_average, float minimum_improvement);
};

#include "controller.tpp"
#endif