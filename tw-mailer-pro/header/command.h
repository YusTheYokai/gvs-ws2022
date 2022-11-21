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
        Command(std::function<void(std::vector<std::string>&)> command);
        std::function<void(std::vector<std::string>&)> getCommand();

    private:
        std::function<void(std::vector<std::string>&)> command;
};

#endif // COMMAND
