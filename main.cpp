#include <iostream>

#include "log_writter.hpp"

int main(void)
{
    LogWriter logger("log_file.txt");

    std::cout << "This wont be logged\n";

    logger.start();

    std::cout << "This is std::cout \n";
    std::cerr << "This is std::cerr \n";

    throw std::runtime_error("This error should be written in the log file");

    // Not reached
    logger.stop();
    logger.close();

    return 0;
}