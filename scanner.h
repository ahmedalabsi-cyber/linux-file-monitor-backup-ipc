/*
 * File: scanner.h
 * Description: Header file for directory scanner module
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "file_info.h"

// Function prototypes
int scan_directory(const char *dir_path, FileInfo *files, int max_files);
int is_txt_file(const char *filename);
int get_file_details(const char *full_path, FileInfo *info);

#endif // SCANNER_H
