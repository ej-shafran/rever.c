#include "raylib.h"
#include "reverc.h"

#define SQUARE_SIZE 100

void draw_square(Reverc_Context ctx, size_t y, size_t x)
{
	int pixel_x = x * SQUARE_SIZE;
	int pixel_y = y * SQUARE_SIZE;

	Rectangle rec = { .x = pixel_x,
			  .y = pixel_y,
			  .width = SQUARE_SIZE,
			  .height = SQUARE_SIZE };

	float stone_radius = ((float)SQUARE_SIZE / 2) - 5;
	int center_x = pixel_x + (SQUARE_SIZE / 2);
	int center_y = pixel_y + (SQUARE_SIZE / 2);

	DrawRectangleLinesEx(rec, 3, BLACK);

	Reverc_CellState cell = GET_CELL_AT(ctx, y, x);
	switch (cell) {
	case REVERC_CELL_STATE_EMPTY:
		break;
	case REVERC_CELL_STATE_WHITE: {
		DrawCircle(center_x, center_y, stone_radius, WHITE);
	} break;
	case REVERC_CELL_STATE_BLACK: {
		DrawCircle(center_x, center_y, stone_radius, BLACK);
	} break;
	}
}

void draw_move(Reverc_Context *ctx, size_t move_index)
{
	int x = ctx->moves[move_index].x;
	int y = ctx->moves[move_index].y;
	int pixel_x = x * SQUARE_SIZE;
	int pixel_y = y * SQUARE_SIZE;

	float stone_radius = ((float)SQUARE_SIZE / 2) - 5;
	int center_x = pixel_x + (SQUARE_SIZE / 2);
	int center_y = pixel_y + (SQUARE_SIZE / 2);
	DrawCircle(center_x, center_y, stone_radius, YELLOW);

	Vector2 mouse = GetMousePosition();
	bool in_bounds_x = mouse.x >= pixel_x &&
			   mouse.x < pixel_x + SQUARE_SIZE;
	bool in_bounds_y = mouse.y >= pixel_y &&
			   mouse.y < pixel_y + SQUARE_SIZE;
	if (in_bounds_x && in_bounds_y &&
	    IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
		reverc_make_move(ctx, move_index + 1);
	}
}

int main(void)
{
	Reverc_Context ctx = reverc_context_new();

	InitWindow(800, 800, "rever.c");

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(DARKGREEN);

		if (ctx.move_count == 0) {
			int width = MeasureText("GAME OVER!", 40);
			DrawText("GAME OVER!", (800 / 2) - (width / 2),
				 800 / 2 - 20, 40, BLACK);
		} else {
			for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
				for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
					draw_square(ctx, y, x);
				}
			}

			for (size_t i = 0; i < ctx.move_count; ++i) {
				draw_move(&ctx, i);
			}
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
