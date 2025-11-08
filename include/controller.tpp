#include "controller.h"
#include <sstream>

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
    std::cin >> input;
    return resolveCommand(input);
}

template<typename T>
bool Controller<T>::resolveCommand(std::string input)
{
    std::string arg;
    std::stringstream ss(input);
    
    if (std::getline(ss, arg, ' ')) 
    {
        if (arg == "marco")
        {
            std::cout << "polo" << std::flush;
        }
        else if (arg == "quit")
        {
            return false;
        }
        else
        {
            std::cout << "Command \"" << input << "\" not recognized. Try again." << std::flush;
        }
    }

    return true;
}

// void restart();
// void save();
// void load();

// void printStats();
// void viewCurrent();
// void viewAll();

// void evolve(int generations);
// void evolveFor(float seconds);
// void evolveUntil(float target_fitness);
// void evolveUntilStagnatesFor(int generations, float minimum_improvement);
// void evolveUntilStagnatesFor(float seconds, float minimum_improvement);