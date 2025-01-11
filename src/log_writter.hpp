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

class LogWriter : public std::streambuf {
public:
    LogWriter(const std::string &filepath);
    ~LogWriter();

    void start();
    void stop();
    void close();

protected:
    virtual int overflow(int c) override;
    virtual int sync() override;
    virtual std::streamsize xsputn(const char* s, std::streamsize n);

private:
    std::string filepath;
    std::ofstream file_stream;
    std::streambuf* cout_buffer = nullptr;
    std::streambuf* cerr_buffer = nullptr;

    std::string get_current_datetime() const;
    std::string get_log_header() const;

    static void handle_exception();
    static std::string demangle(const char* name);
    std::terminate_handler old_terminate;
};