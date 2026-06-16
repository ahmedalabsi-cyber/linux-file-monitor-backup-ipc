/*
 * File: file_info.h
 * Description: Header file for file information structure and constants
 */

#ifndef FILE_INFO_H
#define FILE_INFO_H

#include <time.h>

// Macros for constants
#define MAX_FILES 100
#define MAX_PATH_LENGTH 512
#define MAX_FILENAME_LENGTH 256
#define BUFFER_SIZE 4096

// File information structure
typedef struct {
    char filename[MAX_FILENAME_LENGTH];
    char full_path[MAX_PATH_LENGTH];
    long size;
    time_t modified_time;
    int is_backed_up;
} FileInfo;

// Function prototypes
void init_file_info(FileInfo *info);
void display_file_info(const FileInfo *info);

#endif // FILE_INFO_H
