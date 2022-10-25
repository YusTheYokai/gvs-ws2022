#include <string>
#include <functional>

class Command {

    public:
        Command(std::string name, std::string description, std::function<bool(int, char[])> command);

        std::string getName();
        std::string getDescription();
        std::function<bool(int, char[])> getCommand();
        
    private:
        std::string name;
        std::string description;
        std::function<bool(int, char[])> command;
    
};
