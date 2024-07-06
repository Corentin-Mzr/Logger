#include "log_writter.hpp"

LogWriter::LogWriter(const std::string &filepath) : filepath(filepath)
{

    file_stream.open(filepath, std::ios::binary);
    if (!file_stream.is_open())
        throw std::runtime_error("Could not open " + filepath);
}

LogWriter::~LogWriter()
{
    stop();
    close();
}

void LogWriter::start()
{
    old_terminate = std::set_terminate(handle_exception);

    cout_buffer = std::cout.rdbuf();
    cerr_buffer = std::cerr.rdbuf();

    std::cout.rdbuf(this);
    std::cerr.rdbuf(this);
}

void LogWriter::stop()
{
    std::set_terminate(old_terminate);

    if (cout_buffer)
    {
        std::cout.rdbuf(cout_buffer);
        cout_buffer = nullptr;
    }

    if (cerr_buffer)
    {
        std::cerr.rdbuf(cerr_buffer);
        cerr_buffer = nullptr;
    }
}

void LogWriter::close()
{
    if (file_stream.is_open())
        file_stream.close();
}

int LogWriter::overflow(int c) {
        if (c == EOF) {
            return !EOF;
        } else {
            int const result1 = file_stream.rdbuf()->sputc(c);
            int const result2 = cout_buffer->sputc(c);
            return result1 == EOF || result2 == EOF ? EOF : c;
        }
}

int LogWriter::sync() {
    int const result1 = file_stream.rdbuf()->pubsync();
    int const result2 = cout_buffer->pubsync();
    return result1 == 0 && result2 == 0 ? 0 : -1;
}

std::streamsize LogWriter::xsputn(const char* s, std::streamsize n) {
    std::string log_entry = get_log_header() + std::string(s, n);
    std::streamsize const r1 = file_stream.rdbuf()->sputn(log_entry.c_str(), log_entry.size());
    std::streamsize const r2 = cout_buffer->sputn(s, n);
    return (r1 == EOF || r2 == EOF) ? EOF : n;
}

std::string LogWriter::get_current_datetime() const
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;

    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");

    return ss.str();
}

std::string LogWriter::get_log_header() const
{
    std::string header = "[" + get_current_datetime() + "] ";
    return header; 
}

std::string LogWriter::demangle(const char* name)
{
    int status;
    char* demangled_name = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    if (status == 0) {
        std::string demangled = demangled_name;
        std::free(demangled_name);
        return demangled;
    } else {
        // Demangling failed, return the original name
        return name;
    }
}

void LogWriter::handle_exception()
{
    std::exception_ptr exptr = std::current_exception();
    if (exptr != nullptr)
    {
        try {
            std::rethrow_exception(exptr);
        } catch (const std::exception& e) {
            std::string exception = "EXCEPTION CAUGHT: " + demangle(typeid(e).name()) + " - " + e.what() + "\n";
            std::cerr << exception;
        }
    }
    std::abort();
}
