#include "reverc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define MATRIX_AT(m, y, x, size) m[((y) * (size)) + (x)]
#define BLACK_AT(board, y, x) \
	((board).black & (1ULL << (((y) * REVERC_BOARD_SIZE) + (x))))
#define WHITE_AT(board, y, x) \
	((board).white & (1ULL << (((y) * REVERC_BOARD_SIZE) + (x))))
#define GET_BOARD_AT(board, y, x)                          \
	(BLACK_AT(board, y, x) ? REVERC_CELL_STATE_BLACK : \
	 WHITE_AT(board, y, x) ? REVERC_CELL_STATE_WHITE : \
				 REVERC_CELL_STATE_EMPTY)

#define SET_BOARD_AT(board, y, x, state)                                      \
	do {                                                                  \
		if (state == REVERC_CELL_STATE_BLACK) {                       \
			(board).black |=                                      \
				(1ULL << (((y) * REVERC_BOARD_SIZE) + (x)));  \
			(board).white &=                                      \
				~(1ULL << (((y) * REVERC_BOARD_SIZE) + (x))); \
		} else if (state == REVERC_CELL_STATE_WHITE) {                \
			(board).white |=                                      \
				(1ULL << (((y) * REVERC_BOARD_SIZE) + (x)));  \
			(board).black &=                                      \
				~(1ULL << (((y) * REVERC_BOARD_SIZE) + (x))); \
		}                                                             \
	} while (0)

static int DIRECTIONS[8][2] = { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 },
				{ 1, 1 },   { 1, 0 },  { 1, -1 }, { 0, -1 } };

static uint64_t count_bits(uint64_t n)
{
	uint64_t c = n - ((n >> 1) & 0x7777777777777777ULL) -
		     ((n >> 2) & 0x3333333333333333ULL) -
		     ((n >> 3) & 0x1111111111111111);
	c = ((c + (c >> 4)) & 0x0f0f0f0f0f0f0f0fULL) * 0x0101010101010101ULL;

	return c >> 56;
}

static void calculate_move_changes(Reverc_Context ctx, Reverc_Move *m)
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
			Reverc_CellState cell = GET_BOARD_AT(ctx.board, y, x);

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
}

static void calculate_moves(Reverc_Context *ctx)
{
	ctx->move_count = 0;

	for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
		for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
			if (GET_BOARD_AT(ctx->board, y, x) !=
			    REVERC_CELL_STATE_EMPTY)
				continue;

			Reverc_Move m = {
				.x = x,
				.y = y,
				.changes_count = 0,
				.changes = { 0 },
			};
			calculate_move_changes(*ctx, &m);
			if (m.changes_count > 0) {
				ctx->moves[ctx->move_count++] = m;
			}
		}
	}
}

Reverc_Context reverc_context_new(void)
{
	Reverc_Context ctx = { 0 };

	size_t first = (REVERC_BOARD_SIZE / 2) - 1;
	size_t second = first + 1;
	SET_BOARD_AT(ctx.board, first, first, REVERC_CELL_STATE_BLACK);
	SET_BOARD_AT(ctx.board, second, first, REVERC_CELL_STATE_WHITE);
	SET_BOARD_AT(ctx.board, first, second, REVERC_CELL_STATE_WHITE);
	SET_BOARD_AT(ctx.board, second, second, REVERC_CELL_STATE_BLACK);

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
			SET_BOARD_AT(ctx.board, y, x,
				     GET_BOARD_AT(other.board, y, x));
		}
	}
	for (size_t i = 0; i < other.move_count; ++i) {
		ctx.moves[i] = other.moves[i];
	}
	return ctx;
}

bool reverc_make_move(Reverc_Context *ctx, size_t move_number)
{
	if (move_number == 0 || move_number - 1 >= ctx->move_count)
		return false;

	Reverc_CellState self = ctx->is_black ? REVERC_CELL_STATE_BLACK :
						REVERC_CELL_STATE_WHITE;
	Reverc_Move m = ctx->moves[move_number - 1];
	SET_BOARD_AT(ctx->board, m.y, m.x, self);
	for (size_t i = 0; i < m.changes_count; ++i) {
		SET_BOARD_AT(ctx->board, m.changes[i][0], m.changes[i][1],
			     self);
	}
	ctx->is_black = !ctx->is_black;
	calculate_moves(ctx);
	return true;
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

			switch (GET_BOARD_AT(ctx.board, y, x)) {
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

	size_t white_count = count_bits(ctx.board.white);
	size_t black_count = count_bits(ctx.board.black);
	if (black_count > white_count) {
		printf("BLACK WINS!\n");
	} else if (white_count > black_count) {
		printf("WHITE WINS!\n");
	} else {
		printf("TIE!\n");
	}
	return false;
}
