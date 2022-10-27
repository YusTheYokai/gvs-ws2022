#include <functional>
#include <string>
#include <vector>

class Command {

    public:
        Command(std::string name, std::string description, std::function<void(int, std::vector<std::string>&)> command);

        std::string getName();
        std::string getDescription();
        std::function<void(int, std::vector<std::string>&)> getCommand();

    private:
        std::string name;
        std::string description;
        std::function<void(int, std::vector<std::string>&)> command;
};
