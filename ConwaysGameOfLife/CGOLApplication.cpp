#include "CGOLApplication.h"
#include "Renderer.h"
#include "SDL.h"

#include <iostream>
#include <algorithm>
#include <chrono>

bool CGOLApplication::m_IsRunning = true;

CGOLApplication::CGOLApplication(Renderer* renderer, int cellSize = 20)
	: m_pGrid(nullptr)
	, m_pRenderer(renderer)
	, m_CellSize(cellSize)
	, m_TickDelay(0.3f)
	, m_TickDelayIncrease(0.05f)
	, m_CurrentDelay(0.f)
	, m_RunningSimulation(false)
{
}

void CGOLApplication::Run()
{
	Initialize();

	//store the time from the last frame
	auto timeLastFrame = std::chrono::high_resolution_clock::now();
	while (m_IsRunning)
	{
		//Get the difference in seconds between now and the last frame
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - timeLastFrame).count();

		//Basic game loop
		//Handle Input -> Update -> Render
		HandleInput();
		Update(deltaTime);
		m_pRenderer->Render();

		timeLastFrame = currentTime;
	}

	//Cleanup all the resources
	Cleanup();
}

void CGOLApplication::SetTickDelay(float seconds)
{
	m_TickDelay = abs(seconds);
}

void CGOLApplication::Initialize()
{
	//Create a new grid where the grid full covers the screen with cells
	m_pGrid = new Grid{ m_pRenderer->GetWindowWidth() / m_CellSize, m_pRenderer->GetWindowHeight() / m_CellSize, m_CellSize };
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

	//Get the distance in pixels how far the mouse position is from the far left of the cell
	//Subtract xOffset from the mouse position to get the x position of the cell in screen space
	int xOffset = position.x % cellSize;
	int x = position.x - xOffset;

	//Get the distance in pixels how far the mouse position is from the top of the cell
	//Subtract yOffset from the mouse position to get the y position of the cell in screen space
	int yOffset = position.y % cellSize;
	int y = position.y - yOffset;

	//Divide the x and y by the cellSize to get the actual position in the grid
	m_pGrid->ToggleCell(x / cellSize, y / cellSize);
}

void CGOLApplication::RunSimulation()
{
	//Get the current state of the grid
	std::vector<Cell> currentGrid = m_pGrid->GetCellsCopy();
	int width = m_pGrid->GetWidth();
	int height = m_pGrid->GetHeight();

	//Loop over all the cells in the copied grid
	std::for_each(currentGrid.begin(), currentGrid.end(), [this, &currentGrid, width, height](const Cell& cell)
		{
			//Calculate the index for this cell
			//Using that find the number of neighbours this cell has
			int idx = cell.position.x + cell.position.y * width;
			int nrOfNeighbours = GetNrOfAliveNeighbours(currentGrid, idx, width, height);

			if (cell.alive)
			{
				//If the cell is alive and either has less than 2 neighbours or more than 3, it dies
				//Update the state in the non-copied grid
				if (nrOfNeighbours < 2)
					m_pGrid->ToggleCell(cell.position);
				else if (nrOfNeighbours > 3)
					m_pGrid->ToggleCell(cell.position);
			}
			else
			{
				//If the cell is dead and has exactly 3 neighbours, it becomes alive
				//Update the state in the non-copied grid
				if (nrOfNeighbours == 3)
					m_pGrid->ToggleCell(cell.position);
			}
		}
	);
}

int CGOLApplication::GetNrOfAliveNeighbours(const std::vector<Cell>& grid, int index, int gridWidth, int gridHeight)
{
	int neighbourCount = 0;
	int totalCells = int(grid.size());

	//Indices for the top, bottom, left and right neighbouring cells
	int idxLeft = index - 1;
	int idxRight = index + 1;
	int idxUp = index - gridWidth;
	int idxDown = index + gridWidth;

	auto FindAliveNeighbours = [this, totalCells, gridHeight, gridWidth, &grid, &neighbourCount](int movedIdx, int index)
	{
		//Check if the given index is valid and if it's actually a neighbour of the evaluated cell
		if (ValidIndex(movedIdx, totalCells) && OnSameRow(grid[movedIdx].position.y, grid[index].position.y, gridHeight))
		{
			//Get the index of the cell above and below the neighbour cell
			int idxTop = movedIdx - gridWidth;
			int idxBottom = movedIdx + gridWidth;

			//Check if the indices are valid and alive, if so add to the neighbourCount
			if (grid[movedIdx].alive)
				++neighbourCount;

			if (ValidIndex(idxTop, totalCells) && grid[idxTop].alive)
				++neighbourCount;

			if (ValidIndex(idxBottom, totalCells) && grid[idxBottom].alive)
				++neighbourCount;
		}
	};

	//Check for living neighbours
	FindAliveNeighbours(idxLeft, index);
	FindAliveNeighbours(idxRight, index);

	//Check if the indices are valid and alive, if so add to the neighbourCount
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
				//If the left mouse button is pressed check which cell it clicked on and toggle the cell.
				int x, y;
				SDL_GetMouseState(&x, &y);
				ClickedOnCell(glm::ivec2{ x, y });
			}
			break;

		case SDL_KEYUP:
			if (e.key.keysym.sym == SDLK_SPACE)
			{
				//If space is pressed, toggle if the simulation is running or not
				ToggleRunningSimulation();
			}
			else if (e.key.keysym.sym == SDLK_KP_ENTER || e.key.keysym.sym == SDLK_RETURN)
			{
				//If enter is press, toggle wether the grid is visible or not
				m_pRenderer->ToggleGrid();
			}
			else if (e.key.keysym.sym == SDLK_BACKSPACE)
			{
				//If backspace is pressed, set all cells in the grid to dead
				m_pGrid->ClearGrid();
			}
			else if (e.key.keysym.sym == SDLK_UP)
			{
				//Slow down the speed of the simulation
				IncreaseTickDelay(m_TickDelayIncrease);
			}
			else if (e.key.keysym.sym == SDLK_DOWN)
			{
				//Speed up the speed of the simulation
				IncreaseTickDelay(-m_TickDelayIncrease);
			}
			break;

		case SDL_QUIT:
			//If the close button on the window is hit, exit the application
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
	//If the simulation is running, check if this frame the grid should update.
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