#include "reverc.h"
#include <stdbool.h>
#include <stddef.h>

#define MATRIX_AT(m, y, x, size) m[((y) * (size)) + (x)]
#define BOARD_AT(board, y, x) MATRIX_AT(board, y, x, REVERC_BOARD_SIZE)

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
			Reverc_CellState cell = BOARD_AT(ctx.board, y, x);

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
			if (BOARD_AT(ctx->board, y, x) !=
			    REVERC_CELL_STATE_EMPTY)
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
			BOARD_AT(ctx.board, y, x) = REVERC_CELL_STATE_EMPTY;
		}
	}

	size_t first = (REVERC_BOARD_SIZE / 2) - 1;
	size_t second = first + 1;
	BOARD_AT(ctx.board, first, first) = REVERC_CELL_STATE_BLACK;
	BOARD_AT(ctx.board, second, first) = REVERC_CELL_STATE_WHITE;
	BOARD_AT(ctx.board, first, second) = REVERC_CELL_STATE_WHITE;
	BOARD_AT(ctx.board, second, second) = REVERC_CELL_STATE_BLACK;

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
			BOARD_AT(ctx.board, y, x) = BOARD_AT(other.board, y, x);
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
	BOARD_AT(ctx->board, m.y, m.x) = self;
	for (size_t i = 0; i < m.changes_count; ++i) {
		BOARD_AT(ctx->board, m.changes[i][0], m.changes[i][1]) = self;
	}
	ctx->is_black = !ctx->is_black;
	calculate_moves(ctx);
}

bool reverc_context_report(Reverc_Context ctx)
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

			switch (BOARD_AT(ctx.board, y, x)) {
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

	printf("GAME OVER!\n%s wins!", ctx.is_black ? "White" : "Black");
	return false;
}
