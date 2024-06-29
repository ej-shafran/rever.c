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

bool print_board(Reverc_Context ctx)
{
	printf("┌");
	for (size_t i = 0; i < REVERC_BOARD_SIZE; i++) {
		for (size_t j = 0; j < 4; j++)
			printf("─");
		if (i + 1 != REVERC_BOARD_SIZE)
			printf("┬");
	}
	printf("┐\n");
	for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
		for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
			if (x > 0)
				printf(" │ ");
			else
				printf("│ ");

			switch (GET_CELL_AT(ctx, y, x)) {
			case REVERC_CELL_STATE_EMPTY: {
				ssize_t move = -1;
				for (size_t i = 0; i < ctx.move_count; ++i) {
					Reverc_Move m = ctx.moves[i];
					if (m.x == x && m.y == y) {
						move = i;
						break;
					}
				}

				if (move == -1) {
					printf("  ");
				} else {
					printf("%2zu", move + 1);
				}
			} break;
			case REVERC_CELL_STATE_WHITE: {
				printf("⚪");
			} break;
			case REVERC_CELL_STATE_BLACK: {
				printf("⚫");
			} break;
			}
		}
		printf(" │\n");

		if (y + 1 != REVERC_BOARD_SIZE) {
			printf("├");
			for (size_t i = 0; i < REVERC_BOARD_SIZE; i++) {
				for (size_t j = 0; j < 4; j++)
					printf("─");
				if (i + 1 != REVERC_BOARD_SIZE)
					printf("┼");
			}
			printf("┤\n");
		}
	}
	printf("└");
	for (size_t i = 0; i < REVERC_BOARD_SIZE; i++) {
		for (size_t j = 0; j < 4; j++)
			printf("─");
		if (i + 1 != REVERC_BOARD_SIZE)
			printf("┴");
	}
	printf("┘\n");

	if (ctx.move_count > 0) {
		printf("%s's turn (%zu moves)\n",
		       ctx.is_black ? "Black" : "White", ctx.move_count);
		return true;
	}

	printf("GAME OVER!\n");

	Reverc_CellState winner = reverc_winner(ctx);
	if (winner == REVERC_CELL_STATE_BLACK) {
		printf("BLACK WINS!\n");
	} else if (winner == REVERC_CELL_STATE_WHITE) {
		printf("WHITE WINS!\n");
	} else {
		printf("TIE!\n");
	}
	return false;
}

int main(int argc, const char **argv)
{
	Reverc_Context ctx = reverc_context_new(argc, argv);

	printf("Type `help` for help\n");

	int exitcode = 0;
	bool quit = false;
	while (!quit) {
		if (!print_board(ctx))
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
