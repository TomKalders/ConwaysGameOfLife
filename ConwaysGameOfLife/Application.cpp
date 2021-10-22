#include "Application.h"
#include "Renderer.h"

#include <chrono>
#include <iostream>

bool Application::m_IsRunning = true;

Application::Application(Renderer* m_pRenderer)
	: m_pRenderer(m_pRenderer)
{
}

void Application::Run()
{
	try
	{
		bool result = Initialize();

		//store the time from the last frame
		auto timeLastFrame = std::chrono::high_resolution_clock::now();
		while (m_IsRunning && result)
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

		if (!result)
		{
			throw std::exception("Application failed to initialize");
		}
	}
	catch (std::exception exception)
	{
		std::cout << exception.what() << std::endl;
		//Cleanup();
	}
	catch (...)
	{
		std::cout << "An unknown execption has been caught in Application\n";
		//Cleanup();
	}

	//Cleanup all the resources
	Cleanup();
}

void Application::QuitApplication()
{
	m_IsRunning = false;
}