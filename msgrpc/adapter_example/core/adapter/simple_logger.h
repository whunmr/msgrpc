#ifndef PROJECT_SIMPLE_LOGGER_H
#define PROJECT_SIMPLE_LOGGER_H

#include <msgrpc/core/adapter/logger.h>
#include <msgrpc/util/singleton.h>

namespace demo {

    struct SimpleLogger : msgrpc::Logger, msgrpc::Singleton<SimpleLogger> {
        virtual void set_log_level(severity_level level)  override { level_ = level; }
        virtual bool is_logging_for(severity_level level) override { return level_ <= level; }

        virtual void log_trace   (const char *log) override { std::cout << "[TRACE]  " << log << std::endl; }
        virtual void log_debug   (const char *log) override { std::cout << "[DEBUG]  " << log << std::endl; }
        virtual void log_info    (const char *log) override { std::cout << "[INFO]   " << log << std::endl; }
        virtual void log_warning (const char *log) override { std::cout << "[WARNING]" << log << std::endl; }
        virtual void log_error   (const char *log) override { std::cout << "[ERROR]  " << log << std::endl; }
        virtual void log_fatal   (const char *log) override { std::cout << "[FATAL]  " << log << std::endl; }

    private:
        severity_level level_ = { msgrpc::Logger::info };
    };
}

namespace msgrpc {
    Logger& get_logger() {
        return demo::SimpleLogger::instance();
    }
}

#endif //PROJECT_SIMPLE_LOGGER_H
