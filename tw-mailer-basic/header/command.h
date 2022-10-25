#include <string>
#include <functional>

class Command {

    public:
        Command(std::string name, std::string description, std::function<bool(int, char[])> command);

    private:
        std::string name;
        std::string description;
        std::function<bool(int, char[])> command;
    
};
