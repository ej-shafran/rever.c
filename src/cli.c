#include "reverc.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int exitCode = 0;

#define MAIN_LOOP_QUIT(code)     \
	do {                     \
		quit = true;     \
		exitCode = code; \
		goto _end;       \
	} while (0)

bool PrintBoard(RevercContext ctx)
{
	printf("┌");
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		for (size_t j = 0; j < 4; j++)
			printf("─");
		if (i + 1 != BOARD_SIZE)
			printf("┬");
	}
	printf("┐\n");
	for (size_t y = 0; y < BOARD_SIZE; ++y) {
		for (size_t x = 0; x < BOARD_SIZE; ++x) {
			if (x > 0)
				printf(" │ ");
			else
				printf("│ ");

			switch (GET_CELL_AT(ctx, y, x)) {
			case CELL_EMPTY: {
				ssize_t move = -1;
				for (size_t i = 0; i < ctx.movesCount; ++i) {
					Move m = ctx.moves[i];
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
			case CELL_WHITE: {
				printf("⚪");
			} break;
			case CELL_BLACK: {
				printf("⚫");
			} break;
			}
		}
		printf(" │\n");

		if (y + 1 != BOARD_SIZE) {
			printf("├");
			for (size_t i = 0; i < BOARD_SIZE; i++) {
				for (size_t j = 0; j < 4; j++)
					printf("─");
				if (i + 1 != BOARD_SIZE)
					printf("┼");
			}
			printf("┤\n");
		}
	}
	printf("└");
	for (size_t i = 0; i < BOARD_SIZE; i++) {
		for (size_t j = 0; j < 4; j++)
			printf("─");
		if (i + 1 != BOARD_SIZE)
			printf("┴");
	}
	printf("┘\n");

	if (ctx.movesCount > 0) {
		printf("%s's turn (%zu moves)\n",
		       ctx.isBlack ? "Black" : "White", ctx.movesCount);
		return true;
	}

	printf("GAME OVER!\n");

	CellState winner = GetWinner(ctx);
	if (winner == CELL_BLACK) {
		printf("BLACK WINS!\n");
	} else if (winner == CELL_WHITE) {
		printf("WHITE WINS!\n");
	} else {
		printf("TIE!\n");
	}
	return false;
}

bool PlayerMove(RevercContext *ctx)
{
	bool quit = false;

	char *command = NULL;
	size_t n = 0;

	ssize_t nread = getline(&command, &n, stdin);
	if (nread < 0 || command[nread - 1] != '\n') {
		fprintf(stderr, "\nFailed to read from stdin\n");
		quit = true;
		exitCode = 1;
		goto _end;
	}
	command[nread - 1] = '\0';

	if (strcmp(command, "quit") == 0) {
		quit = true;
	} else if (strcmp(command, "help") == 0) {
		printf("possible commands: \n");
		printf("  help - print this help information\n");
		printf("  [number] - play the nth suggested move\n");
		printf("  quit - exit the game immediately\n");
	} else if (isdigit(command[0])) {
		char *endptr = NULL;
		size_t moveNumber = strtoul(command, &endptr, 10);

		if (*endptr != '\0' || !MakeMove(ctx, moveNumber - 1))
			fprintf(stderr, "Invalid move.\n");
	} else {
		fprintf(stderr, "Invalid command '%s'\n", command);
	}

_end:
	if (command)
		free(command);

	return quit;
}

int main(int argc, const char **argv)
{
	RevercContext ctx = NewContext(argc, argv);
	ParseError err = GetParseError();
	switch (err.kind) {
	case NO_ERROR: {
	} break;
	case UNKNOWN_OPTION: {
		fprintf(stderr, "unknown option '%s'", err.data);
		return 1;
	} break;
	case TWO_PLAYER_PLAY_AS: {
		fprintf(stderr, "cannot specify who to play as for two-player");
		return 1;
	} break;
	}

	printf("Type `help` for help\n");

	bool quit = false;
	while (!quit) {
		if (!PrintBoard(ctx))
			break;

		if (IsPlayerMove(ctx)) {
			if (PlayerMove(&ctx))
				break;
		} else {
			 MakeMove(&ctx, GetComputerMoveIndex(ctx));
		}
	}

	return exitCode;
}
