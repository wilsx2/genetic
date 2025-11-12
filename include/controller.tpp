#include "controller.h"
#include <sstream>
#include <cctype>
#include <chrono>

template<typename T>
Controller<T>::Controller(GeneticAlgorithm<T>&& ga)
    : ga_(ga)
{
    commands_["stats"]  = [&](ArgumentList args){ printStats(); };
    commands_["restart"]= [&](ArgumentList args){ restart(); };
    commands_["save"]   = [&](ArgumentList args){ save(); };
    commands_["load"]   = [&](ArgumentList args)
    {
        if (args.size() == 1)
            load(args[0]); 
        else
            std::cout << "Received " << args.size() << " arguments, expected 1";
    };
    commands_["quit"]   = [&](ArgumentList args){ running_ = false; };
    commands_["view-population"] = [&](ArgumentList args){ viewPopulation(); };
    commands_["view-best"] = [&](ArgumentList args){ viewBest(); };

    commands_["evolve"] = [&](ArgumentList args)
    {
        if (args.size() == 0)
        {
            evolveGenerations(1);
        }
        else if (args.size() == 1)
        {
            try
            {
                evolveGenerations(std::stoi(args[0]));
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else
        {
            errorUnexpectedNumberOfArguments(0, 1, args.size());
        }
    };

    commands_["evolve-seconds"] = [&](ArgumentList args)
    {
        if (args.size() == 1)
        {
            try
            {
                evolveSeconds(std::stof(args[0]));
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else
        {
            errorUnexpectedNumberOfArguments(1, args.size());
        }
    };

    commands_["evolve-until-fitness"] = [&](ArgumentList args)
    {
        if (args.size() == 1)
        {
            try
            {
                evolveUntilFitness(std::stof(args[0]));
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else
        {
            errorUnexpectedNumberOfArguments(1, args.size());
        }
    };

    commands_["evolve-until-generation"] = [&](ArgumentList args)
    {
        if (args.size() == 1)
        {
            try
            {
                evolveUntilGeneration(std::stoi(args[0]));
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else
        {
            errorUnexpectedNumberOfArguments(1, args.size());
        }
    };

    commands_["evolve-until-stagnant-x-generations"] = [&](ArgumentList args)
    {
        if (args.size() == 2)
        {
            try
            {
                int generations = std::stoi(args[0]);
                try
                {
                    float minimum_improvement = std::stof(args[1]);
                    evolveUntilStagnantForGenerations(generations, minimum_improvement);
                }
                catch (std::invalid_argument& e)
                {
                    errorInvalidArgument(args[1]);
                }
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else if (args.size() == 1)
        {
            try
            {
                evolveUntilStagnantForGenerations(std::stoi(args[0]), 0.05f);
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else
        {
            errorUnexpectedNumberOfArguments(1, 2, args.size());
        }
    };

    commands_["evolve-until-stagnant-x-seconds"] = [&](ArgumentList args)
    {
        if (args.size() == 2)
        {
            try
            {
                float seconds = std::stof(args[0]);
                try
                {
                    float minimum_improvement = std::stof(args[1]);
                    evolveUntilStagnantForSeconds(seconds, minimum_improvement);
                }
                catch (std::invalid_argument& e)
                {
                    errorInvalidArgument(args[1]);
                }
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else if (args.size() == 1)
        {
            try
            {
                evolveUntilStagnantForSeconds(std::stof(args[0]), 0.05f);
            }
            catch (std::invalid_argument& e)
            {
                errorInvalidArgument(args[0]);
            }
        }
        else
        {
            errorUnexpectedNumberOfArguments(1, 2, args.size());
        }
    };
}
template<typename T>
void Controller<T>::errorInvalidArgument(std::string arg)
{
    std::cerr << "Invalid argument \"" << arg << "\"\n";
}

template<typename T>
void Controller<T>::errorUnexpectedNumberOfArguments(std::size_t expected, std::size_t received)
{
    std::cerr << "Expected " << expected << " argument(s), received " << received << "\n";
}

template<typename T>
void Controller<T>::errorUnexpectedNumberOfArguments(std::size_t min, std::size_t max, std::size_t received)
{
    std::cerr << "Expected between " << min << " and " << max << " argument(s), received " << received << "\n";
}

template<typename T>
void Controller<T>::run()
{
    std::string input;
    running_ = true;
    while(running_)
    {
        std::cout << "[" << ga_.getProblem() << "]> ";
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

template<typename T>
void Controller<T>::restart()
{
    ga_.newPopulation(ga_.getPopulation().size());
}

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
    std::cout << "Generation " << ga_.getGeneration() << "\n"
        << "Fittest Score " << ga_.getFittestScore() << "\n"
        << "Population ID " << ga_.getFormattedId() << "\n";
}

template <typename T>
void Controller<T>::viewPopulation()
{
    view_function_(ga_.getFittestOfEachGeneration());
}

template <typename T>
void Controller<T>::viewBest()
{
    view_function_(ga_.getPopulation());
}

template <typename T>
void Controller<T>::evolveGenerations(int generations)
{
    for(int i = 0; i < generations; ++i) ga_.evolve();
}
template <typename T>
void Controller<T>::evolveSeconds(float seconds)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    auto time_elapsed = [start_time]()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
    };
    while (time_elapsed() < seconds)
        ga_.evolve();
}
template <typename T>
void Controller<T>::evolveUntilGeneration(int target_generation)
{
    int current = ga_.getGeneration();
    int difference = target_generation - current;
    if (difference <= 1)
    {
        std::cerr << "Current generation is " << current << ", which is at or past target generation of " << target_generation << "\n";
    }
    else
    {
        evolveGenerations(difference);
    }
}

template <typename T>
void Controller<T>::evolveUntilFitness(float target_fitness)
{
    while (ga_.getFittestScore() < target_fitness)
    {
        ga_.evolve();
    }
}

template <typename T>
void Controller<T>::evolveUntilStagnantForGenerations(int generations, float minimum_improvement)
{
    int stagnant_for = 0;
    float previous_fittest, current_fittest;
    do 
    {
        previous_fittest = ga_.getFittestScore();
        ga_.evolve();
        current_fittest = ga_.getFittestScore();

        if (current_fittest / previous_fittest < 1.f + minimum_improvement)
            ++stagnant_for;
        else
            stagnant_for = 0;
    }
    while (stagnant_for < generations);
}

template <typename T>
void Controller<T>::evolveUntilStagnantForSeconds(float seconds, float minimum_improvement)
{
    auto last_time_not_stangant = std::chrono::high_resolution_clock::now();
    auto time_elapsed = [last_time_not_stangant]()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::seconds>(current_time - last_time_not_stangant).count();
    };

    float previous_fittest, current_fittest;
    do 
    {
        previous_fittest = ga_.getFittestScore();
        ga_.evolve();
        current_fittest = ga_.getFittestScore();

        if (current_fittest / previous_fittest >= 1.f + minimum_improvement)
            last_time_not_stangant = std::chrono::high_resolution_clock::now();
    }
    while (time_elapsed() < seconds);
}