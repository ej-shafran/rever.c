#include "reverc.h"
#include <stdbool.h>
#include <stddef.h>
#include <string.h>

static int DIRECTIONS[8][2] = { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 },
				{ 1, 1 },   { 1, 0 },  { 1, -1 }, { 0, -1 } };

static size_t move_is_valid(Reverc_Context ctx, Reverc_Move *m)
{
	Reverc_CellState other = ctx.is_black ? REVERC_CELL_STATE_WHITE :
						REVERC_CELL_STATE_BLACK;

	for (size_t i = 0; i < 8; i++) {
		int dy = DIRECTIONS[i][0];
		int dx = DIRECTIONS[i][1];

		size_t changes[REVERC_BOARD_SIZE - 1][2] = { 0 };
		size_t changes_count = 0;
		bool seen_other = false;
		for (int x = m->x + dx, y = m->y + dy;
		     x >= 0 && x < REVERC_BOARD_SIZE && y >= 0 &&
		     y < REVERC_BOARD_SIZE;
		     x += dx, y += dy) {
			Reverc_CellState cell = ctx.board[y][x];

			if (cell == REVERC_CELL_STATE_EMPTY)
				break;

			if (cell == other) {
				seen_other = true;
				changes[changes_count][0] = y;
				changes[changes_count++][1] = x;
				continue;
			}

			if (!seen_other)
				break;

			for (size_t i = 0; i < changes_count; ++i) {
				m->changes[m->changes_count + i][0] =
					changes[i][0];
				m->changes[m->changes_count + i][1] =
					changes[i][1];
			}
			m->changes_count += changes_count;
		}
	}

	return m->changes_count;
}

static void calculate_moves(Reverc_Context *ctx)
{
	ctx->move_count = 0;

	for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
		for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
			if (ctx->board[y][x] != REVERC_CELL_STATE_EMPTY)
				continue;

			Reverc_Move m = {
				.x = x,
				.y = y,
				.changes_count = 0,
				.changes = { 0 },
			};
			if (move_is_valid(*ctx, &m)) {
				ctx->moves[ctx->move_count++] = m;
			}
		}
	}
}

Reverc_Context reverc_context_new(void)
{
	Reverc_Context ctx = { 0 };

	for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
		for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
			ctx.board[y][x] = REVERC_CELL_STATE_EMPTY;
		}
	}

	size_t first = (REVERC_BOARD_SIZE / 2) - 1;
	size_t second = first + 1;
	ctx.board[first][first] = REVERC_CELL_STATE_BLACK;
	ctx.board[second][first] = REVERC_CELL_STATE_WHITE;
	ctx.board[first][second] = REVERC_CELL_STATE_WHITE;
	ctx.board[second][second] = REVERC_CELL_STATE_BLACK;

	calculate_moves(&ctx);

	return ctx;
}

Reverc_Context reverc_context_clone(Reverc_Context other)
{
	Reverc_Context ctx = { 0 };
	ctx.is_black = other.is_black;
	ctx.move_count = other.move_count;
	for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
		for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
			ctx.board[y][x] = other.board[y][x];
		}
	}
	for (size_t i = 0; i < other.move_count; ++i) {
		ctx.moves[i] = other.moves[i];
	}
	return ctx;
}

void reverc_make_move(Reverc_Context *ctx, size_t move_number)
{
	Reverc_CellState self = ctx->is_black ? REVERC_CELL_STATE_BLACK :
						REVERC_CELL_STATE_WHITE;
	Reverc_Move m = ctx->moves[move_number - 1];
	ctx->board[m.y][m.x] = self;
	for (size_t i = 0; i < m.changes_count; ++i) {
		ctx->board[m.changes[i][0]][m.changes[i][1]] = self;
	}
	ctx->is_black = !ctx->is_black;
	calculate_moves(ctx);
}
