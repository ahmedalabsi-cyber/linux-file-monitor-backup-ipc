/*
 * File: logger.h
 * Description: Header file for dual logging system
 */

#ifndef LOGGER_H
#define LOGGER_H

// Log levels
#define LOG_INFO 0
#define LOG_ERROR 1
#define LOG_WARNING 2

// Function prototypes
int init_logging(void);
void log_message(int level, const char *message);
void log_info(const char *format, ...);
void log_error(const char *format, ...);
void log_warning(const char *format, ...);
void close_logging(void);

#endif // LOGGER_H
