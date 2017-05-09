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


        virtual void log_trace  (const std::string& log) override { log_trace  (log.c_str()); }
        virtual void log_debug  (const std::string& log) override { log_debug  (log.c_str()); }
        virtual void log_info   (const std::string& log) override { log_info   (log.c_str()); }
        virtual void log_warning(const std::string& log) override { log_warning(log.c_str()); }
        virtual void log_error  (const std::string& log) override { log_error  (log.c_str()); }
        virtual void log_fatal  (const std::string& log) override { log_fatal  (log.c_str()); }

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
