#include "reverc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define MAX_DEPTH 4

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
			Reverc_CellState cell = GET_CELL_AT(ctx, y, x);

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
			break;
		}
	}
}

static void calculate_moves(Reverc_Context *ctx)
{
	ctx->move_count = 0;

	for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
		for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
			if (GET_CELL_AT(*ctx, y, x) != REVERC_CELL_STATE_EMPTY)
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

Reverc_Context reverc_context_new(int argc, const char **argv)
{
	bool is_two_player = false;
	bool player_is_black = true;
	argc -= 1;
	argv += 1;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--two-player") == 0) {
			is_two_player = true;
		} else if (strcmp(argv[i], "--play-as-white") == 0) {
			if (is_two_player) {
				REVERC_WARNING(
					"--play-as-white has no meaning when passing --two-player");
				continue;
			}

			player_is_black = false;
		} else {
			REVERC_WARNING("unrecognized argument '%s'", argv[i]);
		}
	}

	Reverc_Context ctx = {
		.is_black = true,
		.board = { .black = 0x810000000, .white = 0x1008000000 },
		.is_two_player = is_two_player,
		.player_is_black = player_is_black,
		.moves = { 0 },
		.move_count = 0,
	};

	calculate_moves(&ctx);

	return ctx;
}

Reverc_Context reverc_context_clone(Reverc_Context other)
{
	Reverc_Context ctx = { 0 };
	ctx.is_black = other.is_black;
	ctx.move_count = other.move_count;
	ctx.board.white = other.board.white;
	ctx.board.black = other.board.black;
	for (size_t i = 0; i < other.move_count; ++i) {
		ctx.moves[i] = other.moves[i];
	}
	return ctx;
}

bool reverc_is_player_move(Reverc_Context ctx)
{
	return ctx.is_two_player || ctx.is_black == ctx.player_is_black;
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

Reverc_CellState reverc_winner(Reverc_Context ctx)
{
	size_t white_count = count_bits(ctx.board.white);
	size_t black_count = count_bits(ctx.board.black);
	if (black_count > white_count) {
		return REVERC_CELL_STATE_BLACK;
	} else if (white_count > black_count) {
		return REVERC_CELL_STATE_WHITE;
	} else {
		return REVERC_CELL_STATE_EMPTY;
	}
}

typedef struct {
	uint64_t best_score;
	size_t best_index;
} ComputerMoveResult;

ComputerMoveResult get_computer_move_impl(Reverc_Context *ctx, bool is_black,
					  size_t depth);

int calculate_position_score(Reverc_Context *ctx, bool is_black, size_t depth)
{
	if (depth >= MAX_DEPTH) {
		uint64_t black_bits = count_bits(ctx->board.black);
		uint64_t white_bits = count_bits(ctx->board.white);
		return is_black ? black_bits - white_bits :
				  white_bits - black_bits;
	}

	return get_computer_move_impl(ctx, is_black, depth).best_score;
}

ComputerMoveResult get_computer_move_impl(Reverc_Context *ctx, bool is_black,
					  size_t depth)
{
	int best_score = INT_MIN;
	size_t best_index = 0;
	for (size_t i = 0; i < ctx->move_count; ++i) {
		Reverc_Context clone = reverc_context_clone(*ctx);
		reverc_make_move(&clone, i + 1);
		int worth =
			calculate_position_score(&clone, is_black, depth + 1);
		if (worth > best_score) {
			best_score = worth;
			best_index = i;
		}
	}

	return (ComputerMoveResult){ .best_index = best_index,
				     .best_score = best_score };
}

size_t reverc_get_computer_move_index(Reverc_Context *ctx)
{
	return get_computer_move_impl(ctx, ctx->is_black, 0).best_index;
}
