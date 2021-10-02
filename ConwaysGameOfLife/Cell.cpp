#include "Cell.h"

Grid::Grid(int width, int height, int cellSize)
	: m_Width(width)
	, m_Height(height)
	, m_CellSize(cellSize)
{
	if (m_Width <= 0)
		m_Width = 1;

	if (m_Height <= 0)
		m_Height = 1;

	m_Cells.reserve(m_Width * m_Height);

	for (int y{ 0 }; y < m_Height; y++)
	{
		for (int x{ 0 }; x < m_Width; x++)
		{
			int idx = x + y * m_Width;
			bool alive = bool(idx % 2);
			if (bool(y % 2))
				alive = !alive;
			m_Cells.push_back(Cell{ glm::vec2{x, y}, m_CellSize, alive });
		}
	}
}

int Grid::GetWidth()
{
	return m_Width;
}

int Grid::GetHeight()
{
	return m_Height;
}

const std::vector<Cell>& Grid::GetCells()
{
	return m_Cells;
}

Cell::Cell(const glm::vec2& position, int size, bool alive)
	: position(position)
	, size(size)
	, alive(alive)
{
}
