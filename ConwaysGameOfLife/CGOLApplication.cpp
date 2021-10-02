#include "CGOLApplication.h"
#include "Renderer.h"
#include "chrono"
#include "SDL.h"

#include "iostream"

bool CGOLApplication::m_IsRunning = true;

CGOLApplication::CGOLApplication(Renderer* renderer)
	: m_pGrid(nullptr)
	, m_pRenderer(renderer)
{
}

void CGOLApplication::Run()
{
	Initialize();

	auto lastTime = std::chrono::high_resolution_clock::now();

	while (m_IsRunning)
	{
		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();

		Update(deltaTime);
		m_pRenderer->Render();

		lastTime = currentTime;

	}

	Cleanup();
}

void CGOLApplication::Initialize()
{
	int cellSize = 20;
	m_pGrid = new Grid{ m_pRenderer->GetWindowWidth() / cellSize, m_pRenderer->GetWindowHeight() / cellSize, cellSize };
	m_pRenderer->Initialize(m_pGrid);
}

void CGOLApplication::Cleanup()
{
	m_pRenderer->Cleanup();
	delete m_pRenderer;
	delete m_pGrid;
}

void CGOLApplication::QuitApplication()
{
	m_IsRunning = false;
}

void CGOLApplication::Update(float deltaTime)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			std::cout << "Mouse down" << std::endl;
		}
	}
}