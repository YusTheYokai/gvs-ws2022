#include <string>
#include <vector>

class MessageUtils {

    public:

        /**
         * Validates a message received from a socket.
         * Will exit the program if the message is invalid.
         */
        static void validateMessage(int size);

        static void parseMessage(char buffer[], int size, std::vector<std::string>& message);
        
        static std::string toString(std::vector<std::string> message);

    private:
        MessageUtils();
};
