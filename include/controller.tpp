#include "controller.h"
#include <sstream>
#include <cctype>

template<typename T>
Controller<T>::Controller(GeneticAlgorithm<T>&& ga)
    : ga_(ga)
{
    commands_["stats"]  = [&](ArgumentList args){ printStats(); };
    commands_["save"]   = [&](ArgumentList args){ save(); };
    commands_["load"]   = [&](ArgumentList args)
    {
        if (args.size() == 1)
            load(args[0]); 
        else
            std::cout << "Received " << args.size() << " arguments, expected 1";
    };
    commands_["quit"]   = [&](ArgumentList args){ running_ = false; };

    commands_["evolve"] = [&](ArgumentList args){
        if (args.size() == 0)
        {
            evolve(1);
        } 
        else if (args.size() == 1 || (args.size() == 2 && args[1] == "generations"))
        {
            try
            {
                evolve(std::stoi(args[0]));
            }
            catch (std::invalid_argument& e) {}
        }
    };
}

template<typename T>
void Controller<T>::run()
{
    std::string input;
    running_ = true;
    while(running_)
    {
        std::cout << "> ";
        if(!std::getline(std::cin, input)) return;
        executeCommand(input);
    }
}

template<typename T>
void Controller<T>::executeCommand(std::string input)
{
    std::istringstream ss(input);
    std::string command;
    std::vector<std::string> args;

    ss >> command;

    std::string arg;
    while (ss >> arg)
        args.push_back(arg);

    auto it = commands_.find(command);
    if (it != commands_.end())
    {
        it->second(args);
    }
    else
    {
        std::cout << "Command \"" << input << "\" not recognized. Try again\n";
    }
}

// void restart();
template<typename T>
void Controller<T>::save()
{
    ga_.savePopulation();
}

template<typename T>
void Controller<T>::load(std::string id)
{
    ga_.loadPopulation(id);
}

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
void evolve(float seconds);
// void evolveUntil(float target_fitness);
// void evolveUntilStagnatesFor(int generations, float minimum_improvement);
// void evolveUntilStagnatesFor(float seconds, float minimum_improvement);