#include "logger.hpp"

Logger::Logger(const std::string &filepath) : filepath(filepath)
{
    file_stream.open(filepath, std::ios::binary);
    if (!file_stream.is_open())
        throw std::runtime_error("Could not open " + filepath);
}

// Start logging in the file
Logger::~Logger()
{
    stop();
    close();
}

void Logger::start()
{
    if (has_started)
        return;

    old_terminate = std::set_terminate(handle_exception);

    cout_buffer = std::cout.rdbuf();
    cerr_buffer = std::cerr.rdbuf();

    std::cout.rdbuf(this);
    std::cerr.rdbuf(this);

    has_started = true;
}

// Stop logging in the file
void Logger::stop()
{
    if (!has_started)
        return;

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

    has_started = false;
}

// Close the file
void Logger::close()
{
    if (file_stream.is_open())
        file_stream.close();
}

// Log a message
void Logger::log(const std::string &s)
{
    if (has_started)
        std::cout << s + '\n';
}

int Logger::overflow(int c)
{
    if (c == EOF)
    {
        return !EOF;
    }
    else
    {
        const int result1 = file_stream.rdbuf()->sputc(c);
        const int result2 = cout_buffer->sputc(c);
        return result1 == EOF || result2 == EOF ? EOF : c;
    }
}

int Logger::sync()
{
    const int result1 = file_stream.rdbuf()->pubsync();
    const int result2 = cout_buffer->pubsync();
    return result1 == 0 && result2 == 0 ? 0 : -1;
}

std::streamsize Logger::xsputn(const char *s, std::streamsize n)
{
    const std::string log_entry = get_log_header() + std::string(s, n);
    const std::streamsize r1 = file_stream.rdbuf()->sputn(log_entry.c_str(), log_entry.size());
    const std::streamsize r2 = cout_buffer->sputn(s, n);
    return (r1 == EOF || r2 == EOF) ? EOF : n;
}

std::string Logger::get_current_datetime() const
{
    const auto now = std::chrono::system_clock::now();
    const auto in_time_t = std::chrono::system_clock::to_time_t(now);
    const auto since_epoch = now.time_since_epoch();
    const auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(since_epoch) % 1000;
    std::tm time_info;
    std::stringstream ss;

    if (localtime_s(&time_info, &in_time_t) != 0)
        throw std::runtime_error("Failed to get local time");

    ss << std::put_time(&time_info, "%Y-%m-%d %X")
       << "." << std::setfill('0') << std::setw(3) << millis.count();
    return ss.str();
}

std::string Logger::get_log_header() const
{
    return "[" + get_current_datetime() + "] ";
}

std::string Logger::demangle(const char *name)
{
#ifdef __GNUG__
    int status;
    char *demangled_name = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    if (status == 0)
    {
        const std::string demangled = demangled_name;
        std::free(demangled_name);
        return demangled;
    }
    else
    {
        // Demangling failed, return the original name
        return name;
    }
#else
    return name;
#endif
}

void Logger::handle_exception()
{
    const std::exception_ptr exptr = std::current_exception();
    if (exptr != nullptr)
    {
        try
        {
            std::rethrow_exception(exptr);
        }
        catch (const std::exception &e)
        {
            const std::string exception = "EXCEPTION CAUGHT: " + demangle(typeid(e).name()) + " - " + e.what() + "\n";
            std::cerr << exception;
        }
    }
    std::abort();
}
