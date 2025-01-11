#include "some_file.hpp"

void some_function()
{
    std::cout << "Calling some_function() that takes 1s to finish\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "some_function() ended\n";
}