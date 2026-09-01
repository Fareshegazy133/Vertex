#include "raylib.h"

void VertexRuntimeTest()
{
	InitWindow(800, 600, "Vertex Test");

	while (!WindowShouldClose())
	{
		BeginDrawing();

		ClearBackground(BLACK);
		DrawText("Vertex Runtime", 20, 20, 30, WHITE);

		EndDrawing();
	}

	CloseWindow();
}