#ifndef REVERC_H_
#define REVERC_H_

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define BOARD_SIZE 8
// https://jxiv.jst.go.jp/index.php/jxiv/preprint/download/480/1498/1315
#define MAX_MOVE_COUNT 33

#define BLACK_AT(board, y, x) \
	((board).black & (1ULL << (((y) * BOARD_SIZE) + (x))))
#define WHITE_AT(board, y, x) \
	((board).white & (1ULL << (((y) * BOARD_SIZE) + (x))))
#define GET_CELL_AT(ctx, y, x)                      \
	(BLACK_AT((ctx).board, y, x) ? CELL_BLACK : \
	 WHITE_AT((ctx).board, y, x) ? CELL_WHITE : \
				       CELL_EMPTY)

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
	CELL_EMPTY,
	CELL_WHITE,
	CELL_BLACK,
} CellState;

typedef struct {
	uint64_t black;
	uint64_t white;
} Board;

typedef struct {
	size_t x;
	size_t y;

	size_t changes[BOARD_SIZE * BOARD_SIZE - 1][2];
	size_t changesCount;
} Move;

typedef struct {
	bool isBlack;
	bool isTwoPlayer;
	bool playerIsBlack;

	Board board;

	size_t movesCount;
	Move moves[MAX_MOVE_COUNT];
} RevercContext;

RevercContext NewContext(int argc, const char **argv);
RevercContext CloneContext(RevercContext other);
size_t GetComputerMoveIndex(RevercContext ctx);
bool IsPlayerMove(RevercContext ctx);
bool MakeMove(RevercContext *ctx, size_t moveNumber);
CellState GetWinner(RevercContext ctx);

#endif // REVERC_H_
