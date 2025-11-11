#include "controller.h"
#include <sstream>
#include <cctype>

template<typename T>
Controller<T>::Controller(GeneticAlgorithm<T>&& ga)
    : ga_(ga)
{}

template<typename T>
void Controller<T>::beginRunning()
{
    while(takeInput());
}
template<typename T>
bool Controller<T>::takeInput()
{
    std::string input;
    std::cout << ">> ";
    if(!std::getline(std::cin, input)) return false;
    return resolveCommand(input);
}

template<typename T>
bool Controller<T>::resolveCommand(std::string input)
{
    std::istringstream ss(input);
    std::string arg;
    
    if (ss >> arg) 
    {
        if (arg == "stats")
        {
            printStats();

        }
        else if (arg == "evolve")
        {
            int num_generations = 1;
            
            if (ss >> arg)
            {
                try
                {
                    num_generations = std::stoi(arg);
                }
                catch (const std::invalid_argument& e) { }
            }

            evolve(num_generations);
        }
        else if (arg == "quit")
        {
            return false;
        }
        else
        {
            std::cout << "Command \"" << input << "\" not recognized. Try again\n";
        }
    }

    return true;
}

// void restart();
// void save();
// void load();

template<typename T>
void Controller<T>::printStats()
{
    const auto& fittest_of = ga_.getFittestOfEachGeneration(); 
    std::cout << "Generation " << fittest_of.size() << "\n" <<
        "Fittest Score " << fittest_of.front().fitness << "\n";
}
// void viewCurrent();
// void viewAll();

template <typename T>
void Controller<T>::evolve(int generations)
{
    for(int i = 0; i < generations; ++i) ga_.evolve();
}
void evolveFor(float seconds);
// void evolveUntil(float target_fitness);
// void evolveUntilStagnatesFor(int generations, float minimum_improvement);
// void evolveUntilStagnatesFor(float seconds, float minimum_improvement);