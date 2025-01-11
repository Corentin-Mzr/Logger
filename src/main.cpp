#include <iostream>

#include "logger.hpp"
#include "some_file.hpp"

int main()
{
    Logger logger("log_file.txt");
    std::cout << "This wont be logged in the file\n";

    // Start to log
    logger.start();

    // Use directly standard streams
    std::cout << "This is std::cout \n";
    std::cerr << "This is std::cerr \n";

    // Or log() and log_no_newline() functions
    logger.log("This is the log() function");

    // Log in a function (via std::cout)
    some_function();

    // Simulate some uncatched error
    throw std::runtime_error("This error should be written in the log file");

    // Not reached (but file is saved)
    logger.stop();
    logger.close();

    return 0;
}