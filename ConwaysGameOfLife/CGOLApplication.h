#pragma once
#include "glm.hpp"
#include <vector>
#include "Cell.h"

class Renderer;
class Grid;
struct GLFWwindow;

class CGOLApplication final
{
public:
	CGOLApplication(Renderer* renderer, int cellSize);
	CGOLApplication(const CGOLApplication& other) = delete;
	CGOLApplication(CGOLApplication&& other) = delete;
	CGOLApplication& operator=(const CGOLApplication& other) = delete;
	CGOLApplication& operator=(CGOLApplication&& other) = delete;

	void Run();
	void SetTickDelay(float seconds);
	static void QuitApplication();

private:
	Grid* m_pGrid;
	Renderer* m_pRenderer;
	int m_CellSize;
	float m_TickDelay;
	float m_CurrentDelay;
	float m_TickDelayIncrease;
	bool m_RunningSimulation;
	static bool m_IsRunning;

	void Initialize();
	void HandleInput();
	void Update(float deltaTime);
	void Cleanup();

	void ClickedOnCell(const glm::ivec2& position);
	void RunSimulation();
	int GetNrOfAliveNeighbours(const std::vector<Cell>& grid, int index, int gridWidth, int gridHeight);
	bool ValidIndex(int idx, int arraySize);
	bool OnSameRow(int y1, int y2, int height);

	void ToggleRunningSimulation();
	void IncreaseTickDelay(float delay);
};
