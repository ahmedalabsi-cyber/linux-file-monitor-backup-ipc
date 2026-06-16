/*
 * File: scanner.c
 * Description: Implementation of directory scanner module
 * Scans directory for .txt files and collects file information
 */

#include "scanner.h"
#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

/**
 * Check if a file is a .txt file
 * @param filename: Name of the file to check
 * Returns: 1 if .txt file, 0 otherwise
 */
int is_txt_file(const char *filename) {
    size_t len = strlen(filename);
    
    // Need at least 5 characters for ".txt"
    if (len < 5) {
        return 0;
    }
    
    // Check if file ends with .txt (case-insensitive)
    const char *ext = filename + len - 4;
    if (strcasecmp(ext, ".txt") == 0) {
        return 1;
    }
    
    return 0;
}

/**
 * Get detailed information about a file
 * @param full_path: Full path to the file
 * @param info: Pointer to FileInfo structure to populate
 * Returns: 0 on success, -1 on error
 */
int get_file_details(const char *full_path, FileInfo *info) {
    struct stat file_stat;
    
    // Initialize the structure
    init_file_info(info);
    
    // Get file statistics
    if (stat(full_path, &file_stat) != 0) {
        log_error("Failed to get file stats for: %s", full_path);
        return -1;
    }
    
    // Extract filename from full path
    const char *last_slash = strrchr(full_path, '/');
    if (last_slash != NULL) {
        strncpy(info->filename, last_slash + 1, MAX_FILENAME_LENGTH - 1);
        info->filename[MAX_FILENAME_LENGTH - 1] = '\0';
    } else {
        strncpy(info->filename, full_path, MAX_FILENAME_LENGTH - 1);
        info->filename[MAX_FILENAME_LENGTH - 1] = '\0';
    }
    
    // Store full path
    strncpy(info->full_path, full_path, MAX_PATH_LENGTH - 1);
    info->full_path[MAX_PATH_LENGTH - 1] = '\0';
    
    // Store file size
    info->size = file_stat.st_size;
    
    // Store modification time
    info->modified_time = file_stat.st_mtime;
    
    // Mark as not backed up initially
    info->is_backed_up = 0;
    
    return 0;
}

/**
 * Scan a directory for .txt files
 * @param dir_path: Path to directory to scan
 * @param files: Array of FileInfo structures to populate
 * @param max_files: Maximum number of files to store
 * Returns: Number of .txt files found, or -1 on error
 */
int scan_directory(const char *dir_path, FileInfo *files, int max_files) {
    DIR *dir;
    struct dirent *entry;
    int file_count = 0;
    char full_path[MAX_PATH_LENGTH];
    
    log_info("Scanning directory: %s", dir_path);
    
    // Open directory
    dir = opendir(dir_path);
    if (dir == NULL) {
        log_error("Failed to open directory: %s", dir_path);
        return -1;
    }
    
    // Read directory entries
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Check if it's a .txt file
        if (!is_txt_file(entry->d_name)) {
            continue;
        }
        
        // Build full path
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        
        // Check if it's a regular file (not a directory)
        struct stat path_stat;
        if (stat(full_path, &path_stat) != 0 || !S_ISREG(path_stat.st_mode)) {
            continue;
        }
        
        // Get file details
        if (get_file_details(full_path, &files[file_count]) == 0) {
            log_info("Found .txt file: %s (Size: %ld bytes)", 
                    files[file_count].filename, files[file_count].size);
            file_count++;
            
            // Check if we've reached the maximum
            if (file_count >= max_files) {
                log_warning("Maximum file limit reached (%d files)", max_files);
                break;
            }
        }
    }
    
    // Close directory
    closedir(dir);
    
    log_info("Scan complete. Found %d .txt file(s).", file_count);
    return file_count;
}
