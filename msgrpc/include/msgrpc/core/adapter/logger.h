#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H

namespace msgrpc {

    struct Logger {
        enum severity_level { trace, debug, info, warning, error, fatal };

        virtual ~Logger() = default;

        virtual void set_log_level(severity_level level) = 0;
        virtual severity_level get_log_level() = 0;

        virtual bool is_logging_for(severity_level level) = 0;

        //trace, debug, info, warning, error, fatal
        virtual void log_trace  (const char* log) = 0;
        virtual void log_debug  (const char* log) = 0;
        virtual void log_info   (const char* log) = 0;
        virtual void log_warning(const char* log) = 0;
        virtual void log_error  (const char* log) = 0;
        virtual void log_fatal  (const char* log) = 0;
    };
}

#endif //PROJECT_LOGGER_H
