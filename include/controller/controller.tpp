#include "controller.h"
#include <sstream>
#include <cctype>
#include <chrono>
#include <thread>

namespace genetic 
{

template<typename T>
Controller<T>::Controller(GeneticAlgorithm<T>&& ga, ViewCallback view)
    : ga_(ga)
    , view_function_(view)
{
    commands_["quit"] = [&](ArgumentList args){ running_ = false; };
    commands_["exit"] = [&](ArgumentList args){ running_ = false; };
    commands_["stats"] = bindCommand<&Controller::printStats>();
    commands_["restart"] = bindCommand<&Controller::restart>();
    commands_["save"] = bindCommand<&Controller::save>();
    commands_["load"] = bindCommand<&Controller::load>();
    commands_["delete-save"] = bindCommand<&Controller::deleteSave>();
    // TODO: add list-saves
    commands_["delete-all-saves"] = bindCommand<&Controller::deleteAllSaves>();
    commands_["view-generation"] = bindCommand<&Controller::viewGeneration>();
    commands_["view-current"] = bindCommand<&Controller::viewCurrent>();
    commands_["view-best"] = bindCommand<&Controller::viewBest>();
    commands_["evolve"] = bindCommand<&Controller::evolveGenerations>();
    commands_["evolve-seconds"] = bindCommand<&Controller::evolveSeconds>();
    commands_["evolve-until-fitness"] = bindCommand<&Controller::evolveUntilFitness>();
    commands_["evolve-until-generation"] = bindCommand<&Controller::evolveUntilGeneration>();
    commands_["evolve-until-stagnant"] = bindCommand<&Controller::evolveUntilStagnant>();
}

template <typename T>
template <typename V>
V Controller<T>::fromString(const std::string& s) {
    std::istringstream iss(s);
    V value;
    iss >> value;
    if (iss.fail() || !iss.eof()) {
        throw std::invalid_argument("Cannot convert string to target type");
    }
    return value;
}

template<typename T>
template<auto MemberFunc>
typename Controller<T>::CommandCallback Controller<T>::bindCommand()
{
    // This method was written by AI
    return [this](ArgumentList args)
    {
        using Func = decltype(MemberFunc);
        
        // Deduce the argument types from the member function pointer
        constexpr auto invoke_with_args = []<typename R, typename C, typename... Args>(R (C::*)(Args...))
        {
            return [](Controller* self, ArgumentList args_list) -> void
            {
                constexpr size_t N = sizeof...(Args);
                
                if (args_list.size() != N)
                {
                    std::cerr << "Expected " << N << " argument(s), received " << args_list.size() << "\n";
                    return;
                }

                try
                {
                    // Convert arguments using index sequence
                    auto converted = [&]<size_t... I>(std::index_sequence<I...>)
                    {
                        return std::tuple{self->template fromString<Args>(args_list[I])...};
                    }(std::make_index_sequence<N>{});

                    // Call the member function
                    std::apply([self](auto&&... converted_args)
                    {
                        (self->*MemberFunc)(std::forward<decltype(converted_args)>(converted_args)...);
                    }, converted);
                }
                catch (const std::exception&)
                {
                    // Find which argument failed by trying to convert each one
                    [&]<size_t... I>(std::index_sequence<I...>)
                    {
                        (([]<size_t Idx>(Controller* s, const std::string& arg)
                        {
                            try
                            {
                                using ArgType = std::tuple_element_t<Idx, std::tuple<Args...>>;
                                s->template fromString<ArgType>(arg);
                            }
                            catch (const std::exception&)
                            {
                                std::cerr << "Invalid argument \"" << arg << "\"\n";
                            }
                        }.template operator()<I>(self, args_list[I])), ...);
                    }(std::make_index_sequence<N>{});
                }
            };
        }(MemberFunc);

        invoke_with_args(this, args);
    };
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
void Controller<T>::executeCommand(const std::string& input)
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
    ga_.restart();
}

template<typename T>
void Controller<T>::save()
{
    ga_.savePopulation();
}

template<typename T>
void Controller<T>::load(std::string id)
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
void Controller<T>::deleteSave(std::string id)
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

    for (auto& save : saves)
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
void Controller<T>::evolve(EvolutionCondition condition)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    float time_elapsed = 0.f;
    auto calculate_time_elapsed = [start_time, &time_elapsed]()
    {
        auto current_time = std::chrono::high_resolution_clock::now();
        time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count() / 1000.f;
    };

    while (condition(ga_.getPopulation(), time_elapsed))
    {
        ga_.evolve();
        calculate_time_elapsed();

        std::cout << "\033[2K" << "Generation:     " << ga_.getPopulation().numGenerations() << "\n";
        std::cout << "\033[2K" << "Fittest Score:  " << ga_.getPopulation().currentFittestScore() << "\n";
        std::cout << "\033[2K" << "Time Elapsed:   " << time_elapsed << "s\n";
        std::cout << "\x1b[A\x1b[A\x1b[A";
    }
    std::cout << "\n\n\n";
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