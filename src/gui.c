#include "raylib.h"
#include "reverc.h"

#define SQUARE_SIZE 100

#define GAME_OVER_MESSAGE "GAME OVER!"

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

void draw_game_over(Reverc_Context ctx)
{
	int font_size = 40;
	int width = MeasureText(GAME_OVER_MESSAGE, font_size);
	DrawText(GAME_OVER_MESSAGE, (800 / 2) - (width / 2), 800 / 2 - 40,
		 font_size, BLACK);

	Reverc_CellState winner = reverc_winner(ctx);
	const char *wins_message;
	switch (winner) {
	case REVERC_CELL_STATE_EMPTY: {
		wins_message = "IT'S A TIE!";
	} break;
	case REVERC_CELL_STATE_WHITE: {
		wins_message = "WHITE WINS!";
	} break;
	case REVERC_CELL_STATE_BLACK: {
		wins_message = "BLACK WINS!";
	} break;
	}
	width = MeasureText(wins_message, font_size);
	DrawText(wins_message, (800 / 2) - (width / 2), 800 / 2 + 20, font_size,
		 BLACK);
}

int main(int argc, const char **argv)
{
	Reverc_Context ctx = reverc_context_new(argc, argv);

	InitWindow(800, 800, "rever.c");

	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(DARKGREEN);

		if (ctx.move_count > 0) {
			for (size_t y = 0; y < REVERC_BOARD_SIZE; ++y) {
				for (size_t x = 0; x < REVERC_BOARD_SIZE; ++x) {
					draw_square(ctx, y, x);
				}
			}

			for (size_t i = 0; i < ctx.move_count; ++i) {
				draw_move(&ctx, i);
			}
		} else {
			draw_game_over(ctx);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}
