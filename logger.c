/*
 * File: logger.c
 * Description: Implementation of dual logging system
 * Logs to both logs/report.txt and alerts/alert.txt
 */

#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <sys/stat.h>

static FILE *report_log = NULL;
static FILE *alert_log = NULL;

/**
 * Initialize logging system
 * Creates necessary directories and opens log files
 * Returns: 0 on success, -1 on error
 */
int init_logging(void) {
    // Create logs directory if it doesn't exist
    mkdir("logs", 0755);
    mkdir("alerts", 0755);
    
    // Open report log (for all messages)
    report_log = fopen("logs/report.txt", "a");
    if (report_log == NULL) {
        fprintf(stderr, "ERROR: Cannot open logs/report.txt for writing\n");
        return -1;
    }
    
    // Open alert log (for errors and warnings only)
    alert_log = fopen("alerts/alert.txt", "a");
    if (alert_log == NULL) {
        fprintf(stderr, "ERROR: Cannot open alerts/alert.txt for writing\n");
        fclose(report_log);
        return -1;
    }
    
    // Write initialization message
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(report_log, "\n========== Logging Session Started: %s ==========\n", timestamp);
    fflush(report_log);
    
    return 0;
}

/**
 * Log a message at specified level
 * @param level: LOG_INFO, LOG_ERROR, or LOG_WARNING
 * @param message: The message to log
 */
void log_message(int level, const char *message) {
    time_t now = time(NULL);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    const char *level_str;
    switch (level) {
        case LOG_INFO:
            level_str = "INFO";
            break;
        case LOG_ERROR:
            level_str = "ERROR";
            break;
        case LOG_WARNING:
            level_str = "WARNING";
            break;
        default:
            level_str = "UNKNOWN";
    }
    
    // All messages go to report log
    if (report_log != NULL) {
        fprintf(report_log, "[%s] [%s] %s\n", timestamp, level_str, message);
        fflush(report_log);
    }
    
    // Errors and warnings also go to alert log
    if ((level == LOG_ERROR || level == LOG_WARNING) && alert_log != NULL) {
        fprintf(alert_log, "[%s] [%s] %s\n", timestamp, level_str, message);
        fflush(alert_log);
    }
}

/**
 * Log an informational message
 */
void log_info(const char *format, ...) {
    char message[1024];
    va_list args;
    
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    log_message(LOG_INFO, message);
}

/**
 * Log an error message (goes to both logs)
 */
void log_error(const char *format, ...) {
    char message[1024];
    va_list args;
    
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    log_message(LOG_ERROR, message);
}

/**
 * Log a warning message (goes to both logs)
 */
void log_warning(const char *format, ...) {
    char message[1024];
    va_list args;
    
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);
    
    log_message(LOG_WARNING, message);
}

/**
 * Close logging system
 */
void close_logging(void) {
    time_t now = time(NULL);
    char timestamp[64];
    
    if (report_log != NULL) {
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&now));
        fprintf(report_log, "========== Logging Session Ended: %s ==========\n", timestamp);
        fclose(report_log);
        report_log = NULL;
    }
    
    if (alert_log != NULL) {
        fclose(alert_log);
        alert_log = NULL;
    }
}
