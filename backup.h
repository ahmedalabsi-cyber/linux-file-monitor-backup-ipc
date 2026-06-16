/*
 * File: backup.h
 * Description: Header file for backup module
 */

#ifndef BACKUP_H
#define BACKUP_H

#include "file_info.h"

// Function prototypes
int backup_file(const FileInfo *file, const char *backup_dir);
int create_backup_directory(const char *backup_dir);
int copy_file_content(const char *src_path, const char *dest_path);

#endif // BACKUP_H
