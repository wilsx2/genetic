#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ga.h"
#include <string>
#include <functional>
#include <variant>
#include <map>

template<typename T>
class Controller
{
    private:
        using ArgumentList = std::vector<std::string>&;
        using CommandCallback = std::function<void(ArgumentList)>;

        GeneticAlgorithm<T> ga_;
        std::map<std::string, CommandCallback> commands_;
        const std::function<void(const std::vector<Member<T>>&)> view_function_;
        bool running_;

        void executeCommand(std::string input);

        void errorInvalidArgument(std::string arg);
        void errorUnexpectedNumberOfArguments(std::size_t expected, std::size_t received);
        void errorUnexpectedNumberOfArguments(std::size_t expected_min, std::size_t expected_max, std::size_t received);
    
    public:
        Controller(GeneticAlgorithm<T>&& ga, std::function<void(const std::vector<Member<T>>&)>);
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