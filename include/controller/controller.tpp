#include "controller.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <chrono>
#include <thread>

namespace genetic 
{

template<typename T>
Controller<T>::Controller(GeneticAlgorithm<T>&& ga, const ViewCallback& view)
    : ga_(ga)
    , view_function_(view)
    , running_(false)
{
    command_handler_.bind<&Controller::stop>("quit", *this);
    command_handler_.bind<&Controller::stop>("exit", *this);
    command_handler_.bind<&Controller::restart>("restart", *this);
    command_handler_.bind<&Controller::save>("save", *this);
    command_handler_.bind<&Controller::load>("load", *this);
    command_handler_.bind<&Controller::deleteSave>("delete-save", *this);
    command_handler_.bind<&Controller::deleteAllSaves>("delete-all-saves", *this);
    command_handler_.bind<&Controller::listSaves>("list-saves", *this);
    command_handler_.bind<&Controller::printStats>("stats", *this);
    command_handler_.bind<&Controller::viewGeneration>("view-generation", *this);
    command_handler_.bind<&Controller::viewCurrent>("view-current", *this);
    command_handler_.bind<&Controller::viewBest>("view-best", *this);
    command_handler_.bind<&Controller::evolveGenerations>("evolve-generations", *this);
    command_handler_.bind<&Controller::evolveSeconds>("evolve-seconds", *this);
    command_handler_.bind<&Controller::evolveUntilFitness>("evolve-until-fitness", *this);
    command_handler_.bind<&Controller::evolveUntilGeneration>("evolve-until-generation", *this);
    command_handler_.bind<&Controller::evolveUntilStagnant>("evolve-until-stagnant", *this);
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
        command_handler_.execute(input);
    }
}

template<typename T>
void Controller<T>::stop()
{
    running_ = false;
}

template<typename T>
void Controller<T>::restart()
{
    ga_.restart();
}

template<typename T>
void Controller<T>::save()
{
    ga_.savePopulation();
}

template<typename T>
void Controller<T>::load(const std::string& id)
{
    if (ga_.loadPopulation(id))
    {
        std::cout << "Successfully loaded population " << id << "\n";
    }
    else
    {
        std::cout << "Failed to load population " << id << "\n";
    }
}

template<typename T>
void Controller<T>::deleteSave(const std::string& id)
{
    ga_.deleteSave(id);
}

template<typename T>
void Controller<T>::deleteAllSaves()
{
    ga_.deleteAllSaves();
}

template<typename T>
void Controller<T>::listSaves()
{
    std::vector<std::string> saves = ga_.getSaves();

    for (const auto& save : saves)
    {
        std::cout << save << "\n";
    }
}

template<typename T>
void Controller<T>::printStats()
{
    const auto& pop = ga_.getPopulation();
    std::cout   << "Generation:     " << pop.numGenerations() << "\n"
                << "Fittest Score:  " << pop.currentFittestScore() << "\n"
                << "Population ID:  " << pop.formattedId() << "\n";
}

template <typename T>
void Controller<T>::viewGeneration(std::size_t i)
{
    if (i < ga_.getPopulation().numGenerations())
        view_function_(ga_.getPopulation().generation(i).members(), ViewType::Population);
    else
        std::cerr << "Input generation does not exist\n";
}

template <typename T>
void Controller<T>::viewCurrent()
{
    view_function_(ga_.getPopulation().current().members(), ViewType::Population);
}

template <typename T>
void Controller<T>::viewBest()
{
    view_function_(ga_.getPopulation().fittestHistory(), ViewType::Generations);
}

template <typename T>
void Controller<T>::EvolutionDisplay::update(int generation, float fitness, float time)
{
    std::cout << CLEAR_LINE << "Generation:     " << generation << "\n";
    std::cout << CLEAR_LINE << "Fittest Score:  " << fitness << "\n";
    std::cout << CLEAR_LINE << "Time Elapsed:   " << time << "s\n";
    std::cout << MOVE_UP_3;
}

template <typename T>
void Controller<T>::EvolutionDisplay::finalize()
{
    std::cout << "\n\n\n";
}

template <typename T>
void Controller<T>::evolve(EvolutionCondition condition)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    float time_elapsed = 0.f;
    auto calculate_time_elapsed = [start_time, &time_elapsed]()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / 1000.f;
    };

    EvolutionDisplay display;
    while (condition(ga_.getPopulation(), time_elapsed))
    {
        ga_.evolve();
        calculate_time_elapsed();
        display.update(ga_.getPopulation().numGenerations(), 
                      ga_.getPopulation().currentFittestScore(), 
                      time_elapsed);
    }
    display.finalize();
}

template <typename T>
void Controller<T>::evolveGenerations(int generations)
{
    evolveUntilGeneration(ga_.getPopulation().numGenerations() + generations);
}

template <typename T>
void Controller<T>::evolveSeconds(float seconds)
{
    EvolutionCondition cond = [seconds](const PopulationHistory<T>&, float time)
    {
        return time < seconds;
    };

    evolve(cond);
}

template <typename T>
void Controller<T>::evolveUntilGeneration(int target_generation)
{
    EvolutionCondition cond = [target_generation](const PopulationHistory<T>& pop, float)
    {
        return pop.numGenerations() < target_generation;
    };

    evolve(cond);
}

template <typename T>
void Controller<T>::evolveUntilFitness(float target_fitness)
{
    constexpr std::size_t TIMEOUT = 10000;
    int start = ga_.getPopulation().numGenerations();
    EvolutionCondition cond = [target_fitness, start](const PopulationHistory<T>& pop, float)
    {
        return pop.currentFittestScore() < target_fitness && pop.numGenerations() - start < TIMEOUT;
    };
    
    evolve(cond);
}

template <typename T>
void Controller<T>::evolveUntilStagnant(int generations, float minimum_average_improvement)
{
    EvolutionCondition cond =
        [generations, minimum_average_improvement]
        (const PopulationHistory<T>& pop, float)
        {
            if (pop.numGenerations() < generations)
                return true;

            float current_fittest = pop.currentFittestScore();
            float fittest_x_generations_ago = pop.fittestHistory()[pop.numGenerations() - generations].fitness;
            
            float improvement = (current_fittest / fittest_x_generations_ago) - 1.f;
            float avg_improvement = improvement / static_cast<float>(generations);
            return avg_improvement > minimum_average_improvement;
        };

    evolve(cond);
}

}