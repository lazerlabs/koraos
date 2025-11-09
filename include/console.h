#pragma once

#include "common.h"

#define CONSOLE_MAX_CMD_LEN 64
#define CONSOLE_MAX_CMDS 16

// Command handler function type
typedef void (*console_cmd_handler_t)(const char *args);

// Command structure
typedef struct {
    const char *name;
    const char *description;
    console_cmd_handler_t handler;
} console_command_t;

// Initialize the console
void console_init(void);

// Register a new command
int console_register_command(const char *name, const char *description, console_cmd_handler_t handler);

// Run the console loop (blocking)
void console_run(void);

// Print a log message (for system logging)
void console_log(const char *message);

