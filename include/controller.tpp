#include "controller.h"
#include <sstream>
#include <cctype>
#include <chrono>

template<typename T>
Controller<T>::Controller(GeneticAlgorithm<T>&& ga, std::function<void(const std::vector<Member<T>>&)> view)
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
    commands_["evolve-until-stagnant-x-generations"] = bind_command<&Controller::evolveUntilStagnantForGenerations>();
    commands_["evolve-until-stagnant-x-seconds"] = bind_command<&Controller::evolveUntilStagnantForSeconds>();
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
    // This function is written by AI
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
    view_function_(ga_.getPopulation());
}

template <typename T>
void Controller<T>::viewBest()
{
    view_function_(ga_.getFittestOfEachGeneration());
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