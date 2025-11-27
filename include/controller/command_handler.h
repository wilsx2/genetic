#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <string>
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
        template<auto MemberFunc>
        CommandCallback bindCommand();

    public:
        template<auto MemberFunc>
        void assign(const std::string& name);
        void execute(const std::string& input);
}

#endif