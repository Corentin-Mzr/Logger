#pragma once

#include <fstream>
#include <string>
#include <iostream>

#include <chrono>
#include <iomanip>
#include <sstream>

#ifdef __GNUG__
#include <cxxabi.h>
#endif

#ifndef __STDC_LIB_EXT1__
#define __STDC_WANT_LIB_EXT1__ 1
#endif

// Class used to log activity of the program in a file via standard character output (std::cout) and standard character error (std::cerr) streams 
class Logger : public std::streambuf {
public:
    Logger(const std::string &filepath);
    ~Logger();

    // Start logging in the file
    void start();

    // Stop logging in the file
    void stop();

    // Close the file    
    void close();

    // Log a message
    void log(const std::string &s);

protected:
    virtual int overflow(int c) override;
    virtual int sync() override;
    virtual std::streamsize xsputn(const char* s, std::streamsize n);

private:
    std::string filepath;
    std::ofstream file_stream;
    std::streambuf* cout_buffer = nullptr;
    std::streambuf* cerr_buffer = nullptr;
    bool has_started = false;

    std::string get_current_datetime() const;
    std::string get_log_header() const;

    static void handle_exception();
    static std::string demangle(const char* name);
    std::terminate_handler old_terminate;
};