#include "commands.h"
#include <string.h>

#define MAX_COMMANDS 64
static shell_command_t command_table[MAX_COMMANDS];
static int command_count = 0;

int shell_register_command(const char *name, command_fn handler, const char *help_text) {
    if (command_count >= MAX_COMMANDS) return -1;
    command_table[command_count].name = name;
    command_table[command_count].handler = handler;
    command_table[command_count].help_text = help_text;
    command_count++;
    return 0;
}

void shell_dispatch(const char *input) {
    // Split command name from arguments at the first space
    char name[32];
    int i = 0;
    while (input[i] && input[i] != ' ' && i < 31) { name[i] = input[i]; i++; }
    name[i] = '\0';
    const char *args = input[i] == ' ' ? &input[i+1] : &input[i];

    for (int j = 0; j < command_count; j++) {
        if (strcmp(command_table[j].name, name) == 0) {
            command_table[j].handler(args);
            return;
        }
    }
    // Unknown command
    extern void vga_puts(const char*);
    vga_puts("Unknown command: ");
    vga_puts(name);
    vga_puts("\n");
}