#include "controller.h"
#include <sstream>
#include <cctype>
#include <chrono>
#include <thread>

template<typename T>
Controller<T>::Controller(GeneticAlgorithm<T>&& ga, ViewCallback view)
    : ga_(ga)
    , view_function_(view)
{
    // Zero-argument commands
    commands_["stats"] = bind_command<&Controller::printStats>();
    commands_["restart"] = bind_command<&Controller::restart>();
    commands_["save"] = bind_command<&Controller::save>();
    commands_["view-population"] = bind_command<&Controller::viewPopulation>();
    commands_["view-best"] = bind_command<&Controller::viewBest>();
    commands_["quit"] = [&](ArgumentList args){ running_ = false; };
    
    // Commands with arguments
    commands_["load"] = bind_command<&Controller::load>();
    commands_["evolve"] = bind_command<&Controller::evolveGenerations>();
    commands_["evolve-seconds"] = bind_command<&Controller::evolveSeconds>();
    commands_["evolve-until-fitness"] = bind_command<&Controller::evolveUntilFitness>();
    commands_["evolve-until-generation"] = bind_command<&Controller::evolveUntilGeneration>();
    commands_["evolve-until-stagnant"] = bind_command<&Controller::evolveUntilStagnant>();
}

template <typename T>
template <typename V>
V Controller<T>::from_string(const std::string& s) {
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
typename Controller<T>::CommandCallback Controller<T>::bind_command()
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
                        return std::tuple{self->template from_string<Args>(args_list[I])...};
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
                                s->template from_string<ArgType>(arg);
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
    view_function_(ga_.getPopulation(), ViewType::Population);
}

template <typename T>
void Controller<T>::viewBest()
{
    view_function_(ga_.getFittestOfEachGeneration(), ViewType::Generations);
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

    while (condition(ga_, time_elapsed))
    {
        ga_.evolve();
        calculate_time_elapsed();

        std::cout << "\033[2K" << "Generation:     " << ga_.getGeneration() << "\n";
        std::cout << "\033[2K" << "Fitness:        " << ga_.getFittestScore() << "\n";
        std::cout << "\033[2K" << "Time Elapsed:   " << time_elapsed << "s\n";
        std::cout << "\x1b[A\x1b[A\x1b[A";
    }
    std::cout << "\n\n\n";
}

template <typename T>
void Controller<T>::evolveGenerations(int generations)
{
    evolveUntilGeneration(ga_.getGeneration() + generations);
}

template <typename T>
void Controller<T>::evolveSeconds(float seconds)
{
    EvolutionCondition cond = [seconds](const GeneticAlgorithm<T>&, float time)
    {
        return time < seconds;
    };

    evolve(cond);
}

template <typename T>
void Controller<T>::evolveUntilGeneration(int target_generation)
{
    EvolutionCondition cond = [target_generation](const GeneticAlgorithm<T>& ga, float)
    {
        return ga.getGeneration() < target_generation;
    };

    evolve(cond);
}

template <typename T>
void Controller<T>::evolveUntilFitness(float target_fitness)
{
    EvolutionCondition cond = [target_fitness](const GeneticAlgorithm<T>& ga, float)
    {
        return ga.getFittestScore() < target_fitness;
    };
    
    evolve(cond);
}

template <typename T>
void Controller<T>::evolveUntilStagnant(int generations, float minimum_average_improvement)
{
    EvolutionCondition cond =
        [generations, minimum_average_improvement]
        (const GeneticAlgorithm<T>& ga, float)
        {
            if (ga.getGeneration() < generations)
                return true;

            float current_fittest = ga.getFittestScore();
            float fittest_x_generations_ago = ga.getFittestOfEachGeneration()[ga.getGeneration() - generations].fitness;
            
            float improvement = (current_fittest / fittest_x_generations_ago) - 1.f;
            float avg_improvement = improvement / static_cast<float>(generations);
            return avg_improvement < minimum_average_improvement;
        };

    evolve(cond);
}