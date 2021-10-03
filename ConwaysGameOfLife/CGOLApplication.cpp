#include "CGOLApplication.h"
#include "Renderer.h"
#include "SDL.h"

#include <iostream>
#include <algorithm>
#include <chrono>

bool CGOLApplication::m_IsRunning = true;

CGOLApplication::CGOLApplication(Renderer* renderer)
	: m_pGrid(nullptr)
	, m_pRenderer(renderer)
	, m_TickDelay(0.3f)
	, m_TickDelayIncrease(0.05f)
	, m_CurrentDelay(0.f)
	, m_RunningSimulation(false)
{
}

void CGOLApplication::Run()
{
	Initialize();

	auto timeLastFrame = std::chrono::high_resolution_clock::now();
	while (m_IsRunning)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - timeLastFrame).count();

		HandleInput();
		Update(deltaTime);
		m_pRenderer->Render();

		timeLastFrame = currentTime;
	}

	Cleanup();
}

void CGOLApplication::SetTickDelay(float seconds)
{
	m_TickDelay = abs(seconds);
}

void CGOLApplication::Initialize()
{
	int cellSize = 5;
	m_pGrid = new Grid{ m_pRenderer->GetWindowWidth() / cellSize, m_pRenderer->GetWindowHeight() / cellSize, cellSize };
	m_pRenderer->Initialize(m_pGrid);
}

void CGOLApplication::Cleanup()
{
	m_pRenderer->Cleanup();
	delete m_pRenderer;
	delete m_pGrid;
}

void CGOLApplication::ClickedOnCell(const glm::ivec2& position)
{
	int cellSize = m_pGrid->GetCellSize();
	int width = m_pGrid->GetWidth();

	int xOffset = position.x % cellSize;
	int x = position.x - xOffset;

	int yOffset = position.y % cellSize;
	int y = position.y - yOffset;

	m_pGrid->ToggleCell(x / cellSize, y / cellSize);
}

void CGOLApplication::RunSimulation()
{
	std::vector<Cell> currentGrid = m_pGrid->GetCellsCopy();
	int width = m_pGrid->GetWidth();
	int height = m_pGrid->GetHeight();

	std::for_each(currentGrid.begin(), currentGrid.end(), [this, &currentGrid, width, height](const Cell& cell)
		{
			int idx = cell.position.x + cell.position.y * width;
			int nrOfNeighbours = GetNrOfAliveNeighbours(currentGrid, idx, width, height);

			if (cell.alive)
			{
				if (nrOfNeighbours < 2)
					m_pGrid->ToggleCell(cell.position);
				else if (nrOfNeighbours > 3)
					m_pGrid->ToggleCell(cell.position);
			}
			else
			{
				if (nrOfNeighbours == 3)
					m_pGrid->ToggleCell(cell.position);
			}
		}
	);
}

int CGOLApplication::GetNrOfAliveNeighbours(const std::vector<Cell>& grid, int index, int gridWidth, int gridHeight)
{
	int neighbourCount = 0;
	int totalCells = grid.size();

	int idxLeft = index - 1;
	int idxRight = index + 1;
	int idxUp = index - gridWidth;
	int idxDown = index + gridWidth;

	auto FindAliveNeighbours = [this, totalCells, gridHeight, gridWidth, &grid, &neighbourCount](int movedIdx, int index)
	{
		if (ValidIndex(movedIdx, totalCells) && OnSameRow(grid[movedIdx].position.y, grid[index].position.y, gridHeight))
		{
			int idxTopLeft = movedIdx - gridWidth;
			int idxBottomLeft = movedIdx + gridWidth;

			if (grid[movedIdx].alive)
				++neighbourCount;

			if (ValidIndex(idxTopLeft, totalCells) && grid[idxTopLeft].alive)
				++neighbourCount;

			if (ValidIndex(idxBottomLeft, totalCells) && grid[idxBottomLeft].alive)
				++neighbourCount;
		}
	};

	FindAliveNeighbours(idxLeft, index);
	FindAliveNeighbours(idxRight, index);

	if (ValidIndex(idxUp, totalCells) && grid[idxUp].alive)
		++neighbourCount;
	if (ValidIndex(idxDown, totalCells) && grid[idxDown].alive)
		++neighbourCount;

	return neighbourCount;
}

bool CGOLApplication::OnSameRow(int idx1, int idx2, int height)
{
	return (idx1 % height == idx2 % height);
}

void CGOLApplication::ToggleRunningSimulation()
{
	m_RunningSimulation = !m_RunningSimulation;
}

void CGOLApplication::IncreaseTickDelay(float delay)
{
	float lowCap = 0.05f;
	float highCap = 1.f;

	m_TickDelay += delay;
	if (m_TickDelay < lowCap)
		m_TickDelay = lowCap;

	if (m_TickDelay > highCap)
		m_TickDelay = highCap;
}


bool CGOLApplication::ValidIndex(int idx, int arraySize)
{
	return (idx > 0 && idx < arraySize - 1);
}

void CGOLApplication::HandleInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				int x, y;
				SDL_GetMouseState(&x, &y);
				ClickedOnCell(glm::ivec2{ x, y });
			}
			break;

		case SDL_KEYUP:
			if (e.key.keysym.sym == SDLK_SPACE)
			{
				ToggleRunningSimulation();
			}
			else if (e.key.keysym.sym == SDLK_KP_ENTER || e.key.keysym.sym == SDLK_RETURN)
			{
				m_pRenderer->ToggleGrid();
			}
			else if (e.key.keysym.sym == SDLK_BACKSPACE)
			{
				m_pGrid->ClearGrid();
			}
			else if (e.key.keysym.sym == SDLK_UP)
			{
				IncreaseTickDelay(m_TickDelayIncrease);
			}
			else if (e.key.keysym.sym == SDLK_DOWN)
			{
				IncreaseTickDelay(-m_TickDelayIncrease);
			}
			break;

		case SDL_QUIT:
			CGOLApplication::QuitApplication();
			break;
		}
	}
}

void CGOLApplication::QuitApplication()
{
	m_IsRunning = false;
}

void CGOLApplication::Update(float deltaTime)
{
	if (m_RunningSimulation)
	{
		m_CurrentDelay += deltaTime;

		if (m_CurrentDelay >= m_TickDelay)
		{
			RunSimulation();
			m_CurrentDelay -= m_TickDelay;
		}
	}
}