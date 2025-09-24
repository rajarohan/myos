// shell.h - Simple command shell
#ifndef SHELL_H
#define SHELL_H

#define SHELL_BUFFER_SIZE 256
#define MAX_ARGS 16

// Shell functions
void shell_init(void);
void shell_run(void);
void shell_prompt(void);
int shell_read_line(char* buffer, int max_length);
void shell_execute_command(const char* command);

// Command handlers
void cmd_help(void);
void cmd_create(const char* filename);
void cmd_list(void);
void cmd_read(const char* filename);
void cmd_write(const char* filename);
void cmd_delete(const char* filename);
void cmd_clear(void);
void cmd_info(void);
void cmd_mkdir(const char* dirname);
void cmd_rmdir(const char* dirname);
void cmd_cd(const char* path);
void cmd_pwd(void);

#endif