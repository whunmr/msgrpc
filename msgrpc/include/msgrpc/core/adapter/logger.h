#ifndef PROJECT_LOGGER_H
#define PROJECT_LOGGER_H

#include <string>
#include <msgrpc/core/adapter/config.h>
#include <msgrpc/core/adapter/adapter_base.h>

namespace msgrpc {

    struct Logger : AdapterBase {
        enum severity_level { trace, debug, info, warning, error, fatal };

        virtual ~Logger() = default;

        virtual void set_log_level(severity_level level) = 0;
        virtual bool is_logging_for(severity_level level) = 0;

        virtual void log_trace  (const char* format, ...) = 0;
        virtual void log_debug  (const char* format, ...) = 0;
        virtual void log_info   (const char* format, ...) = 0;
        virtual void log_warning(const char* format, ...) = 0;
        virtual void log_error  (const char* format, ...) = 0;
        virtual void log_fatal  (const char* format, ...) = 0;
    };

}

//////////////////////////////////////////////////////////////////////////////////////
#define ___msgrpc_log(level_, ...) \
do {\
    if (msgrpc::Config::instance().logger_->is_logging_for(msgrpc::Logger::level_)) {\
        msgrpc::Config::instance().logger_->log_##level_(__VA_ARGS__);\
    }\
} while(0)

#define ___log_trace(...)   ___msgrpc_log(trace,   __VA_ARGS__)
#define ___log_debug(...)   ___msgrpc_log(debug,   __VA_ARGS__)
#define ___log_info(...)    ___msgrpc_log(info,    __VA_ARGS__)
#define ___log_warning(...) ___msgrpc_log(warning, __VA_ARGS__)
#define ___log_error(...)   ___msgrpc_log(error,   __VA_ARGS__)
#define ___log_fatal(...)   ___msgrpc_log(fatal,   __VA_ARGS__)

#endif //PROJECT_LOGGER_H
