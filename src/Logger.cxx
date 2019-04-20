#include <Logger.h>

namespace logging {

    bool log_error;
    bool log_warning;
    bool log_info;
    bool log_verbose;
    bool log_debug;
    const char * program_name;
    static std::string program_name_str;

    void set_program_name(const char * _program_name) {
        program_name_str = _program_name;
        program_name = program_name_str.c_str();
    }
    void set_log_level(int level) {
        log_error = log_warning = log_info = log_debug = log_verbose = false;
        if (level > 0) log_error = true;
        if (level > 1) log_warning = true;
        if (level > 2) log_info = true;
        if (level > 3) log_debug = true;
        if (level > 4) log_verbose = true;
    }

}

namespace {
    class Initializer {
    public:
        Initializer() {
            const char * level_string = getenv("LOG_LEVEL");
            int log_level = 3;
            if (level_string != NULL) {
                char * endptr = NULL;
                log_level = strtol(level_string, &endptr, 10);
                if (not (*level_string != '\0' and *endptr == '\0')) {
                    std::cout << "Logger: " << "LOG_LEVEL has invalid value '"
                              << level_string << "'. Integer expected."
                              << std::endl;
                    log_level = 3;
                }
            }
            logging::set_program_name("Prog");
            logging::set_log_level(log_level);
        }
    };
    
    static Initializer init;
}
