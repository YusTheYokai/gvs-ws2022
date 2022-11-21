#ifndef CLIENTCOMMAND
#define CLIENTCOMMAND

#include "command.h"

class ClientCommand : public Command {

    public:
        ClientCommand(std::string name,
                      std::string description,
                      bool requiresAuthentication,
                      std::function<void(std::vector<std::string>&)> command,
                      std::function<void(std::vector<std::string>&)> callback);

        std::string getName();
        std::string getDescription();
        bool getRequiresAuthentication();
        std::function<void(std::vector<std::string>&)> getCallback();

    private:
        std::string name;
        std::string description;
        bool requiresAuthentication;
        std::function<void(std::vector<std::string>&)> callback;
};

#endif // CLIENTCOMMAND
