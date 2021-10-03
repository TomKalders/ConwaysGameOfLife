#pragma once
#include "Cell.h"

class Renderer
{
public:
	Renderer() = default;
	virtual ~Renderer() = default;
	Renderer(const Renderer& other) = delete;
	Renderer(Renderer&& other) = delete;
	Renderer& operator=(const Renderer& other) = delete;
	Renderer& operator=(Renderer&& other) = delete;

	virtual void Initialize(Grid* grid) = 0;
	virtual void Render() const = 0;
	virtual void Cleanup() = 0;

	virtual int GetWindowWidth() const = 0;
	virtual int GetWindowHeight() const = 0;

	virtual void ToggleGrid() = 0;
private:
};

