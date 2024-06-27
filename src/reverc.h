#ifndef REVERC_H_
#define REVERC_H_

#include <stdbool.h>
#include <stdio.h>

#define REVERC_BOARD_SIZE 8

typedef enum {
	REVERC_CELL_STATE_EMPTY,
	REVERC_CELL_STATE_WHITE,
	REVERC_CELL_STATE_BLACK,
} Reverc_CellState;

typedef Reverc_CellState Reverc_Board[REVERC_BOARD_SIZE * REVERC_BOARD_SIZE];

typedef struct {
	size_t x;
	size_t y;

	size_t changes[REVERC_BOARD_SIZE * REVERC_BOARD_SIZE - 1][2];
	size_t changes_count;
} Reverc_Move;

typedef struct {
	Reverc_Board board;
	bool is_black;
	Reverc_Move moves[REVERC_BOARD_SIZE * REVERC_BOARD_SIZE];
	size_t move_count;
} Reverc_Context;

Reverc_Context reverc_context_new(void);
Reverc_Context reverc_context_clone(Reverc_Context other);
void reverc_make_move(Reverc_Context *ctx, size_t move_number);
bool reverc_context_report(Reverc_Context ctx);

#endif // REVERC_H_
