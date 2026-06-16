/*
 * File: main.c
 * Description: Main program for file monitoring and backup system
 * Accepts command-line arguments and orchestrates scanning and backup
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "file_info.h"
#include "scanner.h"
#include "backup.h"
#include "logger.h"

/**
 * Print usage information
 */
void print_usage(const char *program_name) {
    printf("Usage: %s <directory_path>\n", program_name);
    printf("\nFile Monitoring and Backup System\n");
    printf("----------------------------------\n");
    printf("Scans the specified directory for .txt files,\n");
    printf("logs their details, and creates backup copies.\n");
    printf("\nArguments:\n");
    printf("  directory_path    Path to directory to scan\n");
    printf("\nExample:\n");
    printf("  %s /home/user/documents\n", program_name);
}

/**
 * Main entry point
 */
int main(int argc, char *argv[]) {
    FileInfo *files = NULL;
    int txt_file_count = 0;
    int success_count = 0;
    int error_count = 0;
    int ret_val = 0;
    
    // Check command-line arguments
    if (argc != 2) {
        fprintf(stderr, "Error: Invalid number of arguments\n");
        print_usage(argv[0]);
        return 1;
    }
    
    // Check for help flag
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_usage(argv[0]);
        return 0;
    }
    
    const char *target_dir = argv[1];
    const char *backup_dir = "backup";
    
    printf("\n========================================\n");
    printf("File Monitoring and Backup System\n");
    printf("========================================\n\n");
    printf("Target Directory: %s\n", target_dir);
    printf("Backup Directory: %s\n\n", backup_dir);
    
    // Initialize logging system
    if (init_logging() != 0) {
        fprintf(stderr, "Failed to initialize logging system\n");
        return 1;
    }
    
    log_info("Program started. Target directory: %s", target_dir);
    
    // Allocate memory for file array dynamically (CLO-3 requirement)
    files = (FileInfo *)malloc(MAX_FILES * sizeof(FileInfo));
    if (files == NULL) {
        log_error("Failed to allocate memory for file array");
        fprintf(stderr, "Memory allocation failed\n");
        close_logging();
        return 1;
    }
    
    log_info("Allocated memory for %d files", MAX_FILES);
    
    // Scan directory for .txt files
    printf("Scanning directory for .txt files...\n");
    txt_file_count = scan_directory(target_dir, files, MAX_FILES);
    
    if (txt_file_count < 0) {
        log_error("Directory scan failed");
        fprintf(stderr, "Error: Failed to scan directory\n");
        ret_val = 1;
        goto cleanup;
    }
    
    if (txt_file_count == 0) {
        log_info("No .txt files found in directory");
        printf("No .txt files found.\n");
        goto cleanup;
    }
    
    printf("Found %d .txt file(s)\n\n", txt_file_count);
    
    // Display file information
    printf("File Details:\n");
    printf("----------------------------------------\n");
    for (int i = 0; i < txt_file_count; i++) {
        display_file_info(&files[i]);
        printf("----------------------------------------\n");
    }
    printf("\n");
    
    // Create backup directory
    printf("Creating backup directory...\n");
    if (create_backup_directory(backup_dir) != 0) {
        log_error("Failed to create backup directory");
        fprintf(stderr, "Error: Failed to create backup directory\n");
        ret_val = 1;
        goto cleanup;
    }
    
    // Backup each file
    printf("\nBacking up files...\n");
    printf("----------------------------------------\n");
    
    for (int i = 0; i < txt_file_count; i++) {
        printf("Backing up: %s ... ", files[i].filename);
        
        if (backup_file(&files[i], backup_dir) == 0) {
            files[i].is_backed_up = 1;
            success_count++;
            printf("OK\n");
        } else {
            error_count++;
            printf("FAILED\n");
            fprintf(stderr, "Warning: Backup failed for %s\n", files[i].filename);
        }
    }
    
    printf("----------------------------------------\n\n");
    
    // Print summary
    printf("\n========================================\n");
    printf("BACKUP SUMMARY\n");
    printf("========================================\n");
    printf("Total files found:    %d\n", txt_file_count);
    printf("Successfully backed:  %d\n", success_count);
    printf("Failed backups:       %d\n", error_count);
    printf("========================================\n\n");
    
    // Log summary
    log_info("Backup complete. Success: %d, Failed: %d", success_count, error_count);
    
    if (error_count > 0) {
        log_warning("%d file(s) could not be backed up. Check errors above.", error_count);
    }
    
cleanup:
    // Free allocated memory
    if (files != NULL) {
        free(files);
        files = NULL;
    }
    
    // Close logging system
    close_logging();
    
    printf("Logs saved to: logs/report.txt\n");
    if (error_count > 0) {
        printf("Alerts saved to: alerts/alert.txt\n");
    }
    printf("\nOperation completed.\n\n");
    
    return ret_val;
}
