/*
 * File: file_info.c
 * Description: Implementation of FileInfo structure utilities
 */

#include "file_info.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

/**
 * Initialize a FileInfo structure
 * @param info: Pointer to FileInfo structure
 */
void init_file_info(FileInfo *info) {
    if (info != NULL) {
        memset(info->filename, 0, MAX_FILENAME_LENGTH);
        memset(info->full_path, 0, MAX_PATH_LENGTH);
        info->size = 0;
        info->modified_time = 0;
        info->is_backed_up = 0;
    }
}

/**
 * Display file information (for debugging/logging)
 * @param info: Pointer to FileInfo structure
 */
void display_file_info(const FileInfo *info) {
    if (info == NULL) {
        printf("FileInfo: NULL\n");
        return;
    }
    
    char time_str[64];
    struct tm *tm_info = localtime(&info->modified_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
    
    printf("File: %s\n", info->filename);
    printf("  Path: %s\n", info->full_path);
    printf("  Size: %ld bytes\n", info->size);
    printf("  Modified: %s\n", time_str);
    printf("  Backed up: %s\n", info->is_backed_up ? "Yes" : "No");
}
