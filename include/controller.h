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
        bool running_;

        void executeCommand(std::string input);
    
    public:
        Controller(GeneticAlgorithm<T>&& ga);
        void run();

        void restart();
        void save();
        void load();

        void printStats();
        void viewCurrent();
        void viewAll();

        void evolve(int generations);
        void evolve(float seconds);
        void evolveUntil(float target_fitness);
        void evolveUntilStagnatesFor(int generations, float minimum_improvement);
        void evolveUntilStagnatesFor(float seconds, float minimum_improvement);
};

#include "controller.tpp"
#endif