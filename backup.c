/*
 * File: backup.c
 * Description: Implementation of backup module
 * Creates backup copies of files with error handling
 */

#include "backup.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

/**
 * Create backup directory if it doesn't exist
 * @param backup_dir: Path to backup directory
 * Returns: 0 on success, -1 on error
 */
int create_backup_directory(const char *backup_dir) {
    struct stat dir_stat;
    
    // Check if directory exists
    if (stat(backup_dir, &dir_stat) == 0) {
        if (S_ISDIR(dir_stat.st_mode)) {
            log_info("Backup directory already exists: %s", backup_dir);
            return 0;
        } else {
            log_error("Path exists but is not a directory: %s", backup_dir);
            return -1;
        }
    }
    
    // Create directory
    if (mkdir(backup_dir, 0755) != 0) {
        log_error("Failed to create backup directory: %s", backup_dir);
        return -1;
    }
    
    log_info("Created backup directory: %s", backup_dir);
    return 0;
}

/**
 * Copy file content from source to destination
 * Uses dynamic memory allocation for buffer
 * @param src_path: Source file path
 * @param dest_path: Destination file path
 * Returns: 0 on success, -1 on error
 */
int copy_file_content(const char *src_path, const char *dest_path) {
    FILE *src_file = NULL;
    FILE *dest_file = NULL;
    char *buffer = NULL;
    size_t bytes_read;
    int ret_val = 0;
    
    // Open source file for reading
    src_file = fopen(src_path, "rb");
    if (src_file == NULL) {
        log_error("Cannot open source file for reading: %s", src_path);
        return -1;
    }
    
    // Open destination file for writing
    dest_file = fopen(dest_path, "wb");
    if (dest_file == NULL) {
        log_error("Cannot open destination file for writing: %s", dest_path);
        fclose(src_file);
        return -1;
    }
    
    // Allocate buffer dynamically (as required by CLO-3)
    buffer = (char *)malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        log_error("Failed to allocate memory for file copy buffer");
        fclose(src_file);
        fclose(dest_file);
        return -1;
    }
    
    // Copy file content in chunks
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, src_file)) > 0) {
        if (fwrite(buffer, 1, bytes_read, dest_file) != bytes_read) {
            log_error("Error writing to destination file: %s", dest_path);
            ret_val = -1;
            break;
        }
    }
    
    // Check for read errors
    if (ferror(src_file)) {
        log_error("Error reading from source file: %s", src_path);
        ret_val = -1;
    }
    
    // Free dynamically allocated memory
    free(buffer);
    
    // Close files
    fclose(src_file);
    fclose(dest_file);
    
    if (ret_val == 0) {
        log_info("Successfully copied file: %s -> %s", src_path, dest_path);
    }
    
    return ret_val;
}

/**
 * Backup a single file to the backup directory
 * @param file: Pointer to FileInfo structure
 * @param backup_dir: Path to backup directory
 * Returns: 0 on success, -1 on error
 */
int backup_file(const FileInfo *file, const char *backup_dir) {
    char dest_path[MAX_PATH_LENGTH];
    
    // Validate input
    if (file == NULL || backup_dir == NULL) {
        log_error("Invalid parameters for backup_file");
        return -1;
    }
    
    // Build destination path
    snprintf(dest_path, sizeof(dest_path), "%s/%s", backup_dir, file->filename);
    
    log_info("Backing up file: %s", file->filename);
    
    // Perform the copy
    if (copy_file_content(file->full_path, dest_path) == 0) {
        return 0;
    } else {
        log_error("Backup failed for file: %s", file->filename);
        return -1;
    }
}
