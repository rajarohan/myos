// filesystem.h - Simple in-memory file system
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>

#define MAX_FILES 64
#define MAX_FILENAME_LENGTH 32
#define MAX_FILE_SIZE 1024
#define MAX_PATH_LENGTH 256
#define FILESYSTEM_MEMORY_SIZE (MAX_FILES * MAX_FILE_SIZE + MAX_FILES * sizeof(struct file_entry))

struct file_entry {
    char name[MAX_FILENAME_LENGTH];
    uint32_t size;
    uint32_t data_offset;
    uint8_t used;
    uint8_t is_directory;
    int parent_index;        // Index of parent directory (-1 for root)
    int first_child_index;   // Index of first child (-1 if no children)
    int next_sibling_index;  // Index of next sibling (-1 if last sibling)
};

struct filesystem {
    struct file_entry files[MAX_FILES];
    uint8_t* data_area;
    uint32_t next_data_offset;
    int current_directory;   // Index of current working directory
    int root_directory;      // Index of root directory
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

// Directory operations
int fs_create_directory(const char* dirname);
int fs_change_directory(const char* path);
int fs_remove_directory(const char* dirname);
int fs_list_directory(const char* path);
void fs_get_current_path(char* buffer, int buffer_size);

// Path operations
int fs_resolve_path(const char* path);
int fs_get_parent_directory(int dir_index);
void fs_get_full_path(int file_index, char* buffer, int buffer_size);

// Helper functions
void fs_print_info(void);

#endif