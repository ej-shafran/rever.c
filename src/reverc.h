#ifndef REVERC_H_
#define REVERC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define REVERC_BOARD_SIZE 8
// https://jxiv.jst.go.jp/index.php/jxiv/preprint/download/480/1498/1315
#define REVERC_MAX_MOVE_COUNT 33

#define BLACK_AT(board, y, x) \
	((board).black & (1ULL << (((y) * REVERC_BOARD_SIZE) + (x))))
#define WHITE_AT(board, y, x) \
	((board).white & (1ULL << (((y) * REVERC_BOARD_SIZE) + (x))))
#define GET_CELL_AT(ctx, y, x)                                   \
	(BLACK_AT((ctx).board, y, x) ? REVERC_CELL_STATE_BLACK : \
	 WHITE_AT((ctx).board, y, x) ? REVERC_CELL_STATE_WHITE : \
				       REVERC_CELL_STATE_EMPTY)

#ifdef INCLUDE_RAYLIB
#include "raylib.h"
#endif

#ifndef REVERC_WARNING
#define REVERC_WARNING(...)                    \
	do {                                   \
		fprintf(stderr, "[WARNING] "); \
		fprintf(stderr, __VA_ARGS__);  \
		fprintf(stderr, "\n");         \
	} while (0)
#endif // REVERC_WARNING

typedef enum {
	REVERC_CELL_STATE_EMPTY,
	REVERC_CELL_STATE_WHITE,
	REVERC_CELL_STATE_BLACK,
} Reverc_CellState;

typedef struct {
	uint64_t black;
	uint64_t white;
} Reverc_Board;

typedef struct {
	size_t x;
	size_t y;

	size_t changes[REVERC_BOARD_SIZE * REVERC_BOARD_SIZE - 1][2];
	size_t changes_count;
} Reverc_Move;

typedef struct {
	bool is_black;
	bool is_two_player;
	bool player_is_black;

	Reverc_Board board;

	size_t move_count;
	Reverc_Move moves[REVERC_MAX_MOVE_COUNT];
} Reverc_Context;

Reverc_Context reverc_context_new(int argc, const char **argv);
Reverc_Context reverc_context_clone(Reverc_Context other);
bool reverc_is_player_move(Reverc_Context ctx);
bool reverc_make_move(Reverc_Context *ctx, size_t move_number);
Reverc_CellState reverc_winner(Reverc_Context ctx);

#endif // REVERC_H_
