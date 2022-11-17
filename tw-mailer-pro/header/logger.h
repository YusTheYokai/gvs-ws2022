#ifndef LOGGER
#define LOGGER

#include <string>

class Logger {

    public:
        static void plain(std::string message);
        static void info(std::string message);
        static void success(std::string message);
        static void warn(std::string message);
        static void error(std::string message);

    private:
        static std::string time();
};

#endif // LOGGER
