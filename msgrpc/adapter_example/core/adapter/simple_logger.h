#ifndef PROJECT_SIMPLE_LOGGER_H
#define PROJECT_SIMPLE_LOGGER_H

#include <msgrpc/core/adapter/logger.h>
#include <msgrpc/util/singleton.h>

namespace demo {

    struct SimpleLogger : msgrpc::Logger, msgrpc::Singleton<SimpleLogger> {
        virtual void set_log_level(severity_level level)  override { level_ = level; }
        virtual bool is_logging_for(severity_level level) override { return level_ <= level; }

#define LOG_WITH_LEVEL(log_level_name)\
        va_list args;\
        va_start(args, format);\
        std::cout << log_level_name << format_string(format, args) << std::endl;\
        va_end(args);


        virtual void log_trace   (const char* format, ...) override { LOG_WITH_LEVEL("[TRACE]  "); }
        virtual void log_debug   (const char* format, ...) override { LOG_WITH_LEVEL("[DEBUG]  "); }
        virtual void log_info    (const char* format, ...) override { LOG_WITH_LEVEL("[INFO]   "); }
        virtual void log_warning (const char* format, ...) override { LOG_WITH_LEVEL("[WARNING]"); }
        virtual void log_error   (const char* format, ...) override { LOG_WITH_LEVEL("[ERROR]  "); }
        virtual void log_fatal   (const char* format, ...) override { LOG_WITH_LEVEL("[FATAL]  "); }

#undef LOG_WITH_LEVEL

        const char* format_string(const char* format, va_list& args) {
            static thread_local char buffer[512];
            vsnprintf(buffer, sizeof(buffer), format, args);
            return buffer;
        }

    private:
        severity_level level_ = { msgrpc::Logger::debug };
    };
}

namespace msgrpc {
    Logger& get_logger() {
        return demo::SimpleLogger::instance();
    }
}

#endif //PROJECT_SIMPLE_LOGGER_H
