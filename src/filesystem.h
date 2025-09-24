// filesystem.h - Simple in-memory file system
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define MAX_FILES 32
#define MAX_FILENAME_LENGTH 32
#define MAX_FILE_SIZE 1024
#define FILESYSTEM_MEMORY_SIZE (MAX_FILES * MAX_FILE_SIZE + MAX_FILES * sizeof(struct file_entry))

struct file_entry {
    char name[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t data_offset;
    uint8_t used;
    uint8_t is_directory;
};

struct filesystem {
    struct file_entry files[MAX_FILES];
    uint8_t* data_area;
    uint32_t next_data_offset;
};

// File system operations
int fs_init(void);
int fs_create_file(const char* filename);
int fs_write_file(const char* filename, const char* data, uint32_t size);
int fs_read_file(const char* filename, char* buffer, uint32_t buffer_size);
int fs_delete_file(const char* filename);
int fs_list_files(void);
int fs_file_exists(const char* filename);
uint32_t fs_get_file_size(const char* filename);

// Helper functions
void fs_print_info(void);

#endif