#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

// 日志级别
typedef enum {
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel;

// 日志文件指针
static FILE *log_file = NULL;
static LogLevel current_log_level = LOG_LEVEL_DEBUG;

// 获取当前时间字符串
const char* get_current_time() {
    static char buffer[20];
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    return buffer;
}

// 设置日志级别
void set_log_level(LogLevel level) {
    current_log_level = level;
}

// 初始化日志系统
int init_log(const char *file_name) {
    log_file = fopen(file_name, "a");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file: %s\n", file_name);
        return -1;
    }
    return 0;
}

// 关闭日志系统
void close_log() {
    if (log_file != NULL) {
        fclose(log_file);
    }
}

// 日志记录函数
void log_message(LogLevel level, const char *format, ...) {
    if (level < current_log_level) {
        return;
    }

    const char *level_strings[] = {"DEBUG", "INFO", "WARN", "ERROR"};
    const char *level_string = level_strings[level];

    va_list args;
    va_start(args, format);

    // 打印到日志文件
    if (log_file != NULL) {
        fprintf(log_file, "[%s] [%s] ", get_current_time(), level_string);
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
    }

    va_end(args);
}

int main() {
    // 初始化日志系统
    int ret = init_log("app.log");
    if (ret < 0) {
    	return -1;
    }

    // 设置日志级别
    set_log_level(LOG_LEVEL_DEBUG);

    // 记录不同级别的日志
    log_message(LOG_LEVEL_DEBUG, "This is a debug message.");
    log_message(LOG_LEVEL_INFO, "This is an info message.");
    log_message(LOG_LEVEL_WARN, "This is a warning message.");
    log_message(LOG_LEVEL_ERROR, "This is an error message.");

    // 关闭日志系统
    close_log();

    return 0;
}
