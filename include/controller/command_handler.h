#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>
#include <vector>
#include <functional>
#include <map>

class CommandHandler
{
    private:
        using ArgumentList = std::vector<std::string>&;
        using CommandCallback = std::function<void(ArgumentList)>;
        std::map<std::string, CommandCallback> commands_;

        template <typename V>
        static V fromString(const std::string& s);

    public:
        template<auto MemberFunc, typename C>
        void bind(const std::string& name, C& instance);
        void execute(const std::string& input);
};

#include "command_handler.hpp"
#endif