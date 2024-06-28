#include "reverc.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAIN_LOOP_QUIT(code)     \
	do {                     \
		quit = true;     \
		exitcode = code; \
		goto _break;     \
	} while (0)

int main(void)
{
	Reverc_Context ctx = reverc_context_new();

	printf("Type `help` for help\n");

	int exitcode = 0;
	bool quit = false;
	while (!quit) {
		if (!reverc_context_report(ctx))
			break;

		char *command = NULL;
		size_t n = 0;

		ssize_t nread = getline(&command, &n, stdin);
		if (nread < 0 || command[nread - 1] != '\n') {
			fprintf(stderr, "\nFailed to read from stdin\n");
			MAIN_LOOP_QUIT(1);
		}
		command[nread - 1] = '\0';

		if (strcmp(command, "quit") == 0) {
			quit = true;
			MAIN_LOOP_QUIT(0);
		} else if (strcmp(command, "help") == 0) {
			printf("possible commands: \n");
			printf("  help - print this help information\n");
			printf("  [number] - play the nth suggested move\n");
			printf("  quit - exit the game immediately\n");
		} else if (isdigit(command[0])) {
			char *endptr = NULL;
			size_t move_number = strtoul(command, &endptr, 10);
			if (*endptr != '\0' ||
			    !reverc_make_move(&ctx, move_number))
				fprintf(stderr, "Invalid move.\n");
		} else {
			fprintf(stderr, "Invalid command '%s'\n", command);
			MAIN_LOOP_QUIT(1);
		}

_break:
		if (command)
			free(command);
	}

	return exitcode;
}
