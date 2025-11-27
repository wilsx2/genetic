#include <functional>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <tuple>

template <typename V>
V CommandHandler::fromString(const std::string& s)
{
    std::istringstream iss(s);
    V value;
    iss >> value;
    if (iss.fail() || !iss.eof()) {
        throw std::invalid_argument("Cannot convert string to target type");
    }
    return value;
}

template<auto MemberFunc, typename C>
void CommandHandler::bind(const std::string& name, C& instance)
{
    // This method was written by AI
    
    commands_[name] = [ref = std::ref(instance)](ArgumentList args)
    {
        using Func = decltype(MemberFunc);
        
        // Deduce the argument types from the member function pointer
        constexpr auto invoke_with_args = []<typename R, typename Class, typename... Args>(R (Class::*)(Args...))
        {
            return [](C& obj, ArgumentList args_list) -> void
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
                        return std::tuple{CommandHandler::fromString<Args>(args_list[I])...};
                    }(std::make_index_sequence<N>{});

                    // Call the member function
                    std::apply([&obj](auto&&... converted_args)
                    {
                        (obj.*MemberFunc)(std::forward<decltype(converted_args)>(converted_args)...);
                    }, converted);
                }
                catch (const std::exception&)
                {
                    // Find which argument failed by trying to convert each one
                    [&]<size_t... I>(std::index_sequence<I...>)
                    {
                        (([]<size_t Idx>(const std::string& arg)
                        {
                            try
                            {
                                using ArgType = std::tuple_element_t<Idx, std::tuple<Args...>>;
                                CommandHandler::fromString<ArgType>(arg);
                            }
                            catch (const std::exception&)
                            {
                                std::cerr << "Invalid argument \"" << arg << "\"\n";
                            }
                        }.template operator()<I>(args_list[I])), ...);
                    }(std::make_index_sequence<N>{});
                }
            };
        }(MemberFunc);

        invoke_with_args(ref.get(), args);
    };
}

void CommandHandler::execute(const std::string& input)
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