#include <n7OS/shell.h>
#include <n7OS/console.h>
#include <n7OS/processus.h>
#include <n7OS/keyboard.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

void handle_command(const char *command)
{
	if (strcmp(command, "help") == 0)
	{
		printf("Available commands:\n");
		printf("  help - Show this help message\n");
		printf("  ps - List all processes\n");
		printf("  poweroff - Power off the system\n");
		printf("  clear - Clear the screen\n");
	}
	else if (strcmp(command, "ps") == 0)
	{
		ps();
	}
	else if (strcmp(command, "poweroff") == 0)
	{
		printf("Powering off...\n");
		shutdown(1);
	}
	else if (strcmp(command, "clear") == 0)
	{
		clean_screen();
	}
	else
	{
		printf("Unknown command: %s\n", command);
	}
}

char *read_command()
{
	static char cmd[256];
	uint8_t head = 0;
	while (1)
	{
		char c = kgetch();
		if (c == 0) // No input available
			continue;

		if (c == '\n' || c == '\r') // Enter key pressed
		{
			cmd[head] = '\0'; // Null-terminate the command
			printf("\n");
			return cmd;
		}
		else if (c == 0x08) // Backspace key pressed
		{
			if (head == 0) // do not allow removing the prompt
			{
				continue;
			}

			cmd[--head] = '\0';
			printf("\b \b");
		}
		else if (head < sizeof(cmd) - 1) // Ensure we don't overflow the buffer
		{
			cmd[head++] = c;
			printf("%c", c); // Echo the character to the console
		}
	}
}

void shell()
{
	printf("Welcome to the n7OS shell!\n");
	printf("Type 'help' for a list of commands.\n");

	while (1)
	{
		printf("n7OS> ");
		char *command = read_command();
		if (strlen(command) == 0)
		{
			continue; // Skip empty commands
		}
		handle_command(command);
	}
}
