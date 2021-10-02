#pragma once
#include <glm.hpp>
#include <vector>

struct Cell;
class Grid final
{
public:
	Grid(int width, int height, int cellSize);
	virtual ~Grid() = default;
	Grid(const Grid & other) = default;
	Grid(Grid && other) = default;
	Grid& operator=(const Grid & other) = default;
	Grid& operator=(Grid && other) = default;

	int GetWidth();
	int GetHeight();
	const std::vector<Cell>& GetCells();
private:
	int m_Width;
	int m_Height;
	int m_CellSize;
	std::vector<Cell> m_Cells;
};

struct Cell
{
public:
	explicit Cell(const glm::vec2& position, int size, bool alive = false);
	
	glm::vec2 position;
	int size;
	bool alive;
};

