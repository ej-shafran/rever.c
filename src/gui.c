#include "raylib.h"
#include "reverc.h"
#include <stdbool.h>

#define COMPUTER_MOVE_DELAY 0.45
#define GAME_OVER_DELAY 1.25

#define DEFAULT_SCREEN_SIZE 800
#define GAME_OVER_MESSAGE "GAME OVER!"
#define SCREEN_TITLE "rever.c"

int GetScreenSize(void)
{
	int width = GetScreenWidth();
	int height = GetScreenHeight();

	return width > height ? height : width;
}

int GetSquareSize(void)
{
	return GetScreenSize() / BOARD_SIZE;
}

Rectangle GetSquareRec(size_t y, size_t x)
{
	int squareSize = GetSquareSize();
	int paddingX = (GetScreenWidth() - GetScreenSize()) / 2;
	int paddingY = (GetScreenHeight() - GetScreenSize()) / 2;
	int pixelX = (x * squareSize) + paddingX;
	int pixelY = (y * squareSize) + paddingY;

	Rectangle rec = { .x = pixelX,
			  .y = pixelY,
			  .width = squareSize,
			  .height = squareSize };
	return rec;
}

void DrawSquare(RevercContext ctx, size_t y, size_t x)
{
	Rectangle rec = GetSquareRec(x, y);
	float stoneRadius = (rec.width / 2) - 5;
	int centerX = rec.x + (rec.width / 2);
	int centerY = rec.y + (rec.height / 2);

	DrawRectangleLinesEx(rec, 3, BLACK);

	CellState cell = GET_CELL_AT(ctx, y, x);
	switch (cell) {
	case CELL_EMPTY: {
	} break;
	case CELL_WHITE: {
		DrawCircle(centerX, centerY, stoneRadius, WHITE);
	} break;
	case CELL_BLACK: {
		DrawCircle(centerX, centerY, stoneRadius, BLACK);
	} break;
	}
}

void DrawBoard(RevercContext ctx)
{
	for (size_t y = 0; y < BOARD_SIZE; ++y) {
		for (size_t x = 0; x < BOARD_SIZE; ++x) {
			DrawSquare(ctx, y, x);
		}
	}
}

bool DrawMove(RevercContext *ctx, size_t move_index)
{
	int x = ctx->moves[move_index].x;
	int y = ctx->moves[move_index].y;
	Rectangle rec = GetSquareRec(y, x);

	float stoneRadius = ((float)rec.width / 2) - 5;
	int centerX = rec.x + (rec.width / 2);
	int centerY = rec.y + (rec.height / 2);
	DrawCircle(centerX, centerY, stoneRadius, YELLOW);

	Vector2 mouse = GetMousePosition();
	bool inBoundsX = mouse.x >= rec.x && mouse.x < rec.x + rec.width;
	bool inBoundsY = mouse.y >= rec.y && mouse.y < rec.y + rec.height;

	if (inBoundsX && inBoundsY &&
	    IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		MakeMove(ctx, move_index + 1);
		return true;
	}

	return false;
}

void DrawGameOver(RevercContext ctx)
{
	int screenWidth = GetScreenWidth();
	int screenHeight = GetScreenHeight();
	int fontSize = 40;
	int width = MeasureText(GAME_OVER_MESSAGE, fontSize);
	DrawText(GAME_OVER_MESSAGE, (screenWidth / 2) - (width / 2),
		 screenHeight / 2 - 40, fontSize, BLACK);

	CellState winner = GetWinner(ctx);
	const char *winsMessage;
	switch (winner) {
	case CELL_EMPTY: {
		winsMessage = "IT'S A TIE!";
	} break;
	case CELL_WHITE: {
		winsMessage = "WHITE WINS!";
	} break;
	case CELL_BLACK: {
		winsMessage = "BLACK WINS!";
	} break;
	}
	width = MeasureText(winsMessage, fontSize);
	DrawText(winsMessage, (screenWidth / 2) - (width / 2),
		 screenHeight / 2 + 20, fontSize, BLACK);
}

int main(int argc, const char **argv)
{
	RevercContext ctx = NewContext(argc, argv);

	InitWindow(DEFAULT_SCREEN_SIZE, DEFAULT_SCREEN_SIZE, SCREEN_TITLE);

	bool gameOver = false;
	ssize_t pendingComputerMove = -1;
	float timer = 0;
	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(DARKGREEN);

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

		if (pendingComputerMove != -1) {
			timer += GetFrameTime();
			if (timer > COMPUTER_MOVE_DELAY) {
				MakeMove(&ctx, pendingComputerMove + 1);
				pendingComputerMove = -1;
				timer = 0;
			}
			goto _next;
		}

		for (size_t i = 0; i < ctx.movesCount; ++i) {
			if (DrawMove(&ctx, i)) {
				pendingComputerMove =
					GetComputerMoveIndex(ctx);
				break;
			}
		}

_next:
		EndDrawing();
	}

	CloseWindow();
	return 0;
}
