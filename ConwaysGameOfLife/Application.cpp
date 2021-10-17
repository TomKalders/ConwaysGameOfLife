#include "Application.h"
#include "Renderer.h"
#include "chrono"

bool Application::m_IsRunning = true;

Application::Application(Renderer* m_pRenderer)
	: m_pRenderer(m_pRenderer)
{
}

void Application::Run()
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

void Application::QuitApplication()
{
	m_IsRunning = false;
}
