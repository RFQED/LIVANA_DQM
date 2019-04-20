#ifndef Logging_h
#define Logging_h

#include <iostream>
#include <string>
#include <cstdlib>

#include <sstream>
#include <typeinfo>

// Logging class
// Stolen from a4 debug.h:
// https://github.com/a4
// https://github.com/a4/a4/blob/master/a4io/src/a4/debug.h

namespace _string_internal {
    std::ostream& operator<<(std::ostream &sstr, const std::type_info& ti);

    static inline std::string str_printf(const char* s) {
        return std::string (s);
    }

    template<typename T, typename... Args>
    std::string str_printf(const char* s, const T& value, const Args&... args) {
        std::string res;
        while (*s) {
            if (*s == '%' && *++s != '%') {
                res += std::string(value);
                return res + str_printf(++s, args...);
            }
            res += char(*s++);
        }
        // Append extra arguments
        return res + std::string(value) + str_printf("", args...);
    }

  inline static void _stream_in(std::stringstream& /*ss*/) {}

    template<typename T, typename... Args>
    void _stream_in(std::stringstream& ss, const T& s, const Args&... args) {
        ss << s;
        _stream_in(ss, args...);
    }

    template<typename... Args>
    std::string str_cat(const Args&... args) {
        std::stringstream ss;
        _stream_in(ss, args...);
        return ss.str();
    }

}

template<typename... Args>
static inline std::string str_printf(const char* s, const Args&... args) {
    return _string_internal::str_printf(s, args...);
}

template<typename... Args>
static inline std::string str_cat(const Args&... args) {
    return _string_internal::str_cat(args...);
}

namespace logging {
  
  void set_program_name(const char * program_name);

    // Override log level given in LOG_LEVEL with verbose/quiet from
    // the command line. Log priorities are:
    // * error = 1
    // * warning = 2
    // * info = 3
    // * debug = 4
    // * verbose = 5
    // Only messages with a priority less or equal to the log level are shown.
    void set_log_level(int level = 3);

    /// Extern booleans that are set by set_log_level or using LOG_LEVEL
    extern bool log_error;
    extern bool log_warning;
    extern bool log_info;
    extern bool log_debug;
    extern bool log_verbose;

    /// Program name that prefixes the logging messages.
    extern const char * program_name;
}

#define LOG_IF(cond, level, levelstr, ...) \
  do {								\
    if (logging::log_ ## level and cond) {			\
      std::cout << logging::program_name << "." levelstr	\
  		<< str_cat( __VA_ARGS__ ) << std::endl;		\
    }								\
  } while (0)


#define ERROR(...) LOG_IF(true, error, "Error: ", __VA_ARGS__ )
#define WARNING(...) LOG_IF(true, warning, "Warning: ", __VA_ARGS__ )
#define INFO(...) LOG_IF(true, info, "Info: ", __VA_ARGS__ )
#define DEBUG(...) LOG_IF(true, debug, "Debug: ", __VA_ARGS__ )
#define VERBOSE(...) LOG_IF(true, verbose, "Verbose: ", __VA_ARGS__ )

#define ERROR_ASSERT(A, ...) LOG_IF(A, error, "Error: ", __VA_ARGS__ )
#define WARNING_ASSERT(A, ...) LOG_IF(A, warning, "Warning: ", __VA_ARGS__ )
#define INFO_ASSERT(A, ...) LOG_IF(A, info, "Info: ", __VA_ARGS__ )
#define DEBUG_ASSERT(A, ...) LOG_IF(A, debug, "Debug: ", __VA_ARGS__ )
#define VERBOSE_ASSERT(A, ...) LOG_IF(A, verbose, "Verbose: ", __VA_ARGS__ )

#define FATAL(...) \
  do {			      \
    std::cout << logging::program_name << ".Fatal: "  << str_cat( __VA_ARGS__ ) << std::endl; \
    exit(1); \
  } while(0)
#define FATAL_ASSERT(A, ...) do { if(!(A)) { FATAL( __VA_ARGS__ ); } } while (0)


#endif
