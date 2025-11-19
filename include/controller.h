#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ga.h"
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

        GeneticAlgorithm<T> ga_;
        std::map<std::string, CommandCallback> commands_;
        const ViewCallback view_function_;
        bool running_;

        template <typename V>
        static V from_string(const std::string& s);
        template<auto MemberFunc>
        CommandCallback bind_command();
        void executeCommand(std::string input);
    
    public:
        Controller(GeneticAlgorithm<T>&& ga, ViewCallback view);
        void run();

        void restart();
        void save();
        void load(std::string id);

        void printStats();
        void viewPopulation();
        void viewBest();

        void evolveGenerations(int generations);
        void evolveSeconds(float seconds);
        void evolveUntilFitness(float target_fitness);
        void evolveUntilGeneration(int target_generation);
        void evolveUntilStagnantForGenerations(int generations, float minimum_improvement);
        void evolveUntilStagnantForSeconds(float seconds, float minimum_improvement);
};

#include "controller.tpp"
#endif