#include "reverc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define MAX_DEPTH 4

#define SET_BOARD_AT(board, y, x, state)                                       \
	do {                                                                   \
		if (state == CELL_BLACK) {                                     \
			(board).black |= (1ULL << (((y) * BOARD_SIZE) + (x))); \
			(board).white &=                                       \
				~(1ULL << (((y) * BOARD_SIZE) + (x)));         \
		} else if (state == CELL_WHITE) {                              \
			(board).white |= (1ULL << (((y) * BOARD_SIZE) + (x))); \
			(board).black &=                                       \
				~(1ULL << (((y) * BOARD_SIZE) + (x)));         \
		}                                                              \
	} while (0)

// Internal declarations

typedef struct {
	uint64_t score;
	size_t index;
} ComputerMoveResult;

static ComputerMoveResult GetBestComputerMove(RevercContext ctx, bool isBlack,
					      size_t depth);
static uint64_t CountBits(uint64_t n);
static void CalculateMoveChanges(RevercContext ctx, Move *m);
static void CalculateMoves(RevercContext *ctx);
static uint64_t CalculatePositionScore(RevercContext ctx, bool isBlack,
				       size_t depth);
static ComputerMoveResult GetBestComputerMove(RevercContext ctx, bool isBlack,
					      size_t depth);

// Internal variables

static int DIRECTIONS[8][2] = { { -1, -1 }, { -1, 0 }, { -1, 1 }, { 0, 1 },
				{ 1, 1 },   { 1, 0 },  { 1, -1 }, { 0, -1 } };
static ParseError err = {
	.kind = NO_ERROR,
	.data = NULL,
};

// Internal functions

static uint64_t CountBits(uint64_t n)
{
	uint64_t c = n - ((n >> 1) & 0x7777777777777777ULL) -
		     ((n >> 2) & 0x3333333333333333ULL) -
		     ((n >> 3) & 0x1111111111111111);
	c = ((c + (c >> 4)) & 0x0f0f0f0f0f0f0f0fULL) * 0x0101010101010101ULL;

	return c >> 56;
}

static void CalculateMoveChanges(RevercContext ctx, Move *m)
{
	CellState other = ctx.isBlack ? CELL_WHITE : CELL_BLACK;

	for (size_t i = 0; i < 8; i++) {
		int dy = DIRECTIONS[i][0];
		int dx = DIRECTIONS[i][1];

		size_t changes[BOARD_SIZE - 1][2] = { 0 };
		size_t changesCount = 0;
		bool seenOther = false;
		for (int x = m->x + dx, y = m->y + dy;
		     x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE;
		     x += dx, y += dy) {
			CellState cell = GET_CELL_AT(ctx, y, x);

			if (cell == CELL_EMPTY)
				break;

			if (cell == other) {
				seenOther = true;
				changes[changesCount][0] = y;
				changes[changesCount++][1] = x;
				continue;
			}

			if (!seenOther)
				break;

			for (size_t i = 0; i < changesCount; ++i) {
				m->changes[m->changesCount + i][0] =
					changes[i][0];
				m->changes[m->changesCount + i][1] =
					changes[i][1];
			}
			m->changesCount += changesCount;
			break;
		}
	}
}

static void CalculateMoves(RevercContext *ctx)
{
	ctx->movesCount = 0;

	for (size_t y = 0; y < BOARD_SIZE; ++y) {
		for (size_t x = 0; x < BOARD_SIZE; ++x) {
			if (GET_CELL_AT(*ctx, y, x) != CELL_EMPTY)
				continue;

			Move m = {
				.x = x,
				.y = y,
				.changesCount = 0,
				.changes = { 0 },
			};
			CalculateMoveChanges(*ctx, &m);
			if (m.changesCount > 0) {
				ctx->moves[ctx->movesCount++] = m;
			}
		}
	}
}

static uint64_t CalculatePositionScore(RevercContext ctx, bool isBlack,
				       size_t depth)
{
	if (depth >= MAX_DEPTH) {
		uint64_t blackBits = CountBits(ctx.board.black);
		uint64_t whiteBits = CountBits(ctx.board.white);
		return isBlack ? blackBits - whiteBits : whiteBits - blackBits;
	}

	return GetBestComputerMove(ctx, isBlack, depth).score;
}

static ComputerMoveResult GetBestComputerMove(RevercContext ctx, bool isBlack,
					      size_t depth)
{
	uint64_t bestScore = 0;
	size_t bestIndex = 0;
	for (size_t i = 0; i < ctx.movesCount; ++i) {
		RevercContext clone = CloneContext(ctx);
		MakeMove(&clone, i);
		uint64_t score =
			CalculatePositionScore(clone, isBlack, depth + 1);
		if (score > bestScore) {
			bestScore = score;
			bestIndex = i;
		}
	}

	return (ComputerMoveResult){ .index = bestIndex, .score = bestScore };
}

// Public functions

RevercContext NewContext(int argc, const char **argv)
{
	bool isTwoPlayer = false;
	bool playerIsBlack = true;
	argc -= 1;
	argv += 1;
	for (int i = 0; i < argc; ++i) {
		if (strcmp(argv[i], "--two-player") == 0) {
			isTwoPlayer = true;
		} else if (strcmp(argv[i], "--play-as-white") == 0) {
			if (isTwoPlayer) {
				err.kind = TWO_PLAYER_PLAY_AS;
				break;
			}

			playerIsBlack = false;
		} else {
			err.kind = UNKNOWN_OPTION;
			err.data = argv[i];
			break;
		}
	}

	RevercContext ctx = {
		.isBlack = true,
		.board = { .black = 0x810000000, .white = 0x1008000000 },
		.isTwoPlayer = isTwoPlayer,
		.playerIsBlack = playerIsBlack,
		.moves = { 0 },
		.movesCount = 0,
	};

	CalculateMoves(&ctx);

	return ctx;
}

ParseError GetParseError(void)
{
	return err;
}

RevercContext CloneContext(RevercContext other)
{
	RevercContext ctx = { 0 };
	ctx.isBlack = other.isBlack;
	ctx.movesCount = other.movesCount;
	ctx.board.white = other.board.white;
	ctx.board.black = other.board.black;
	for (size_t i = 0; i < other.movesCount; ++i) {
		ctx.moves[i] = other.moves[i];
	}
	return ctx;
}

bool IsPlayerMove(RevercContext ctx)
{
	return ctx.isTwoPlayer || ctx.isBlack == ctx.playerIsBlack;
}

bool MakeMove(RevercContext *ctx, size_t moveIndex)
{
	if (moveIndex >= ctx->movesCount)
		return false;

	CellState self = ctx->isBlack ? CELL_BLACK : CELL_WHITE;
	Move m = ctx->moves[moveIndex];
	SET_BOARD_AT(ctx->board, m.y, m.x, self);
	for (size_t i = 0; i < m.changesCount; ++i) {
		SET_BOARD_AT(ctx->board, m.changes[i][0], m.changes[i][1],
			     self);
	}
	ctx->isBlack = !ctx->isBlack;
	CalculateMoves(ctx);
	return true;
}

CellState GetWinner(RevercContext ctx)
{
	size_t whiteCount = CountBits(ctx.board.white);
	size_t blackCount = CountBits(ctx.board.black);
	if (blackCount > whiteCount) {
		return CELL_BLACK;
	} else if (whiteCount > blackCount) {
		return CELL_WHITE;
	} else {
		return CELL_EMPTY;
	}
}

size_t GetComputerMoveIndex(RevercContext ctx)
{
	return GetBestComputerMove(ctx, ctx.isBlack, 0).index;
}
