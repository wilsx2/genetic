#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "ga.h"
#include <functional>

template<typename T>
class Controller
{
    private:
        GeneticAlgorithm<T> ga_;
        std::function<void(std::vector<T>)> view_function_;
    
    public:
        Controller(GeneticAlgorithm<T>&& ga);
        void beginRunning();
        void takeCommand(std::string input);

        void restart();
        void save();
        void load();

        void printStats();
        void viewCurrent();
        void viewAll();

        void evolve(int generations);
        void evolveFor(float seconds);
        void evolveUntil(float target_fitness);
        void evolveUntilStagnatesFor(int generations, float minimum_improvement);
        void evolveUntilStagnatesFor(float seconds, float minimum_improvement);
    };

#endif