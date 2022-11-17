#ifndef COMMAND
#define COMMAND

#include <functional>
#include <string>
#include <vector>

/**
 * Command class for both client and server.
 */
class Command {

    public:
        Command(std::string name, std::string description, std::function<void(std::vector<std::string>&)> command);

        std::string getName();
        std::string getDescription();
        std::function<void(std::vector<std::string>&)> getCommand();

    private:
        std::string name;
        std::string description;
        std::function<void(std::vector<std::string>&)> command;
};

#endif // COMMAND
