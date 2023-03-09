#include "raylib.h"

struct LinkedCell {
	int row;
	int col;
	struct LinkedCell * next;
};

void PlacePellet();

static const int gameRows = 15;
static const int gameCols = 15;
static const int lineThickness = 2;
static const int cellSize = 50;

/* Game state globals */
int windowWidth;
int windowHeight;
int directionX;
int directionY;
float updatesPerMove;
int updateCounter;
int snakeLength;

static const Color lineColor = {0, 0, 0, 64};
static const Color snakeBodyColor = {0, 0, 0, 255 };
static const Color pelletColor = { 255, 0, 0, 64 };

struct LinkedCell * snakeHead;
struct LinkedCell foodPellet;

void InitGlobals() {
	windowWidth = gameCols * cellSize + (gameCols - 1) * lineThickness;
	windowHeight = gameRows * cellSize + (gameRows - 1) * lineThickness;
	updatesPerMove = 30;
	updateCounter = 1;

	snakeHead = (struct LinkedCell *)malloc(sizeof(struct LinkedCell));
	snakeHead->row = gameRows / 2;
	snakeHead->col = gameCols / 2;
	snakeHead->next = NULL;
	
	snakeLength = 1;
	directionX = 0; // No X direction
	directionY = 1; // Move straight down initially
	PlacePellet();
}

void CleanUpMemory() {
	struct LinkedCell * currentCell = snakeHead;
	while (true) {
		struct LinkedCell * next = currentCell->next;
		free(currentCell);
		if (next){
			currentCell = next;
		} else {
			break;
		}
	}
}

void DrawGameGrid() {
	for (int row = 0; row < gameRows - 1; row++) {
		int y = cellSize + row * (lineThickness + cellSize);
		DrawRectangle(0, y, windowWidth, lineThickness, lineColor);
	}

	for (int col = 0; col < gameCols - 1; col++) {
		int x = cellSize + col * (lineThickness + cellSize);
		DrawRectangle(x, 0, lineThickness, windowHeight, lineColor);
	}
}

void DrawCell(struct LinkedCell * cell, Color cellColor) {
	int x = cell->col * (lineThickness + cellSize);
	int y = cell->row * (lineThickness + cellSize);
	DrawRectangle(x, y, cellSize, cellSize, cellColor);
}

void DrawSnake()
{
	struct LinkedCell * currentSnakeCell = snakeHead;
	Color cellColor = snakeBodyColor;

	while (true) {
		DrawCell(currentSnakeCell, cellColor);
		printf("%i\n", cellColor.a);
		if (currentSnakeCell->next) {
			currentSnakeCell = currentSnakeCell->next;
		}
		else {
			break;
		}
		if (cellColor.a > 64) {
			cellColor.a -= 5;
		}
	}
}

void DrawPellet() {
	DrawCell(&foodPellet, pelletColor);
}

bool LinkedCellListHasValue(struct LinkedCell * head, int x, int y) {
	while (true) {
		if (head->row == y && head->col == x) {
			return true;
		}
		else if (head->next) {
			head = head->next;
		}
		else {
			return false;
		}
	}
}

void PlacePellet() {
	while (true) {
		int x = GetRandomValue(0, gameCols - 1);
		int y = GetRandomValue(0, gameRows - 1);

		if (!LinkedCellListHasValue(snakeHead, x, y)) {
			foodPellet.row = y;
			foodPellet.col = x;
			break;
		}
	}
}

void IncreaseSpeed() {
	// Needs to be some sort of asymptotic function
	updatesPerMove -= 0.2f;
}

void GrowSnake(struct LinkedCell * cell) {
	struct LinkedCell * newCell = (struct LinkedCell *)malloc(sizeof(struct LinkedCell));
	newCell->row = cell->row;
	newCell->col = cell->col;
	newCell->next = snakeHead;
	snakeHead = newCell;
	snakeLength += 1;
}

void EndGame() {
	CleanUpMemory();
	InitGlobals();
}

void MoveSnake() {
	struct LinkedCell * currentCell = snakeHead;

	int newX = snakeHead->col + directionX;
	int newY = snakeHead->row + directionY;

	while (true) {
		int savedX = currentCell->col;
		int savedY = currentCell->row;

		currentCell->row = newY;
		currentCell->col = newX;

		if (currentCell->next) {
			currentCell = currentCell->next;
			newX = savedX;
			newY = savedY;
		}
		else {
			break;
		}
	}
}

void UpdateSnake() {
	updateCounter++;
	if (updateCounter < updatesPerMove) { return; }
	updateCounter = 0;

	int nextX = snakeHead->col + directionX;
	int nextY = snakeHead->row + directionY;

	if (nextX == foodPellet.col && nextY == foodPellet.row) {
		GrowSnake(&foodPellet);
		PlacePellet();
		IncreaseSpeed();
		return;
	}

	if (LinkedCellListHasValue(snakeHead, nextX, nextY)) {
		EndGame();
	}

	MoveSnake();
}

void HandleInput() {
	if(IsKeyDown(KEY_LEFT)) {
		directionX = -1;
		directionY = 0;
	}
	else if (IsKeyDown(KEY_RIGHT)) {
		directionX = 1;
		directionY = 0;
	}
	else if (IsKeyDown(KEY_UP)) {
		directionX = 0;
		directionY = -1;
	}
	else if (IsKeyDown(KEY_DOWN)) {
		directionX = 0;
		directionY = 1;
	}
}

void CheckBounds() {
	if ((snakeHead->col < 0) || (snakeHead->row < 0) || (snakeHead->row > gameRows - 1) || (snakeHead->col > gameCols - 1)) {
		EndGame();
	}
}

int main(void)
{
	InitGlobals();
	SetTargetFPS(60);
	InitWindow(windowWidth, windowHeight, "Snek");

	while (!WindowShouldClose())
	{
		HandleInput();
		CheckBounds();
		UpdateSnake();
		BeginDrawing();
			ClearBackground(WHITE);
			DrawGameGrid();
			DrawPellet();
			DrawSnake();
		EndDrawing();
	}

	CloseWindow();

	return 0;
}