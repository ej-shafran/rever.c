#include "reverc.h"

#include <raylib.h>
#include <stdbool.h>
#include <config.h>

int GetBoardSize(void)
{
	int width = GetScreenWidth();
	int height = GetScreenHeight();

	return width > height ? height : width;
}

int GetCellSize(void)
{
	return GetBoardSize() / BOARD_SIZE;
}

Rectangle GetCellRec(size_t y, size_t x)
{
	int cellSize = GetCellSize();
	int boardSize = GetBoardSize();
	int paddingX = (GetScreenWidth() - boardSize) / 2;
	int paddingY = (GetScreenHeight() - boardSize) / 2;

	return (Rectangle){ .x = (x * cellSize) + paddingX,
			    .y = (y * cellSize) + paddingY,
			    .width = cellSize,
			    .height = cellSize };
}

void DrawCell(RevercContext ctx, size_t y, size_t x)
{
	Rectangle rec = GetCellRec(y, x);
	float stoneRadius = (rec.width / 2) - 5;
	int centerX = rec.x + (rec.width / 2);
	int centerY = rec.y + (rec.height / 2);

	DrawRectangleLinesEx(rec, 3, LINE_COLOR);

	CellState cell = GET_CELL_AT(ctx, y, x);
	switch (cell) {
	case CELL_EMPTY: {
	} break;
	case CELL_WHITE: {
		DrawCircle(centerX, centerY, stoneRadius, WHITE_COLOR);
	} break;
	case CELL_BLACK: {
		DrawCircle(centerX, centerY, stoneRadius, BLACK_COLOR);
	} break;
	}
}

void DrawBoard(RevercContext ctx)
{
	for (size_t y = 0; y < BOARD_SIZE; ++y) {
		for (size_t x = 0; x < BOARD_SIZE; ++x) {
			DrawCell(ctx, y, x);
		}
	}
}

bool DrawMove(RevercContext *ctx, size_t moveIndex)
{
	int x = ctx->moves[moveIndex].x;
	int y = ctx->moves[moveIndex].y;
	Rectangle rec = GetCellRec(y, x);

	float stoneRadius = ((float)rec.width / 2) - 5;
	int centerX = rec.x + (rec.width / 2);
	int centerY = rec.y + (rec.height / 2);
	DrawCircle(centerX, centerY, stoneRadius, MOVE_COLOR);

	Vector2 mouse = GetMousePosition();
	bool inBoundsX = mouse.x >= rec.x && mouse.x < rec.x + rec.width;
	bool inBoundsY = mouse.y >= rec.y && mouse.y < rec.y + rec.height;

	if (inBoundsX && inBoundsY &&
	    IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		MakeMove(ctx, moveIndex);
		return true;
	}

	return false;
}

void DrawGameOver(RevercContext ctx)
{
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int width = MeasureText(GAME_OVER_MESSAGE, GAME_OVER_FONT_SIZE);
	DrawText(GAME_OVER_MESSAGE, (screenWidth / 2) - (width / 2),
		 screenHeight / 2 - GAME_OVER_FONT_SIZE, GAME_OVER_FONT_SIZE,
		 TEXT_COLOR);

	CellState winner = GetWinner(ctx);
	const char *winsMessage;
	switch (winner) {
	case CELL_EMPTY: {
		winsMessage = TIE_MESSAGE;
	} break;
	case CELL_WHITE: {
		winsMessage = WHITE_WINS_MESSAGE;
	} break;
	case CELL_BLACK: {
		winsMessage = BLACK_WINS_MESSAGE;
	} break;
	}
	width = MeasureText(winsMessage, GAME_OVER_FONT_SIZE);
	DrawText(winsMessage, (screenWidth / 2) - (width / 2),
		 screenHeight / 2 + 20, GAME_OVER_FONT_SIZE, TEXT_COLOR);
}

int main(int argc, const char **argv)
{
	RevercContext ctx = NewContext(argc, argv);

	ParseError err = GetParseError();
	switch (err.kind) {
	case NO_ERROR: {
	} break;
	case UNKNOWN_OPTION: {
		TraceLog(LOG_ERROR, "unknown option '%s'", err.data);
		return 1;
	} break;
	case TWO_PLAYER_PLAY_AS: {
		TraceLog(LOG_ERROR,
			 "cannot specify who to play as for two-player");
		return 1;
	} break;
	}

	InitWindow(DEFAULT_SCREEN_SIZE, DEFAULT_SCREEN_SIZE, SCREEN_TITLE);

	bool gameOver = false;
	ssize_t pendingComputerMove = -1;
	float timer = 0;
	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(BACKGROUND_COLOR);

		if (gameOver) {
			DrawGameOver(ctx);
			goto _next;
		}

		if (ctx.movesCount == 0) {
			timer += GetFrameTime();
			if (timer > GAME_OVER_DELAY) {
				gameOver = true;
			}
			DrawBoard(ctx);
		}

		DrawBoard(ctx);

		if (!IsPlayerMove(ctx) && pendingComputerMove != -1) {
			timer += GetFrameTime();
			if (timer > COMPUTER_MOVE_DELAY) {
				MakeMove(&ctx, pendingComputerMove);
				pendingComputerMove = -1;
				timer = 0;
			}
			goto _next;
		}

		for (size_t i = 0; i < ctx.movesCount; ++i) {
			if (DrawMove(&ctx, i) && !ctx.isTwoPlayer) {
				pendingComputerMove = GetComputerMoveIndex(ctx);
				break;
			}
		}

_next:
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
