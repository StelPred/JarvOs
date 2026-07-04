#ifndef _SHELL_COMMANDS_H
#define _SHELL_COMMANDS_H

typedef void (*command_fn)(const char *args);

typedef struct {
    const char *name;
    command_fn handler;
    const char *help_text;
} shell_command_t;

// Call this once at boot to add a command to the table
int shell_register_command(const char *name, command_fn handler, const char *help_text);

// Call this with whatever the user (or AI) typed
void shell_dispatch(const char *input);

#endif