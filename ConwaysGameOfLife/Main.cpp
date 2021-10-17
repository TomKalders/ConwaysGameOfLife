#include <windows.h>
#include <iostream>
#include <vld.h>

#include "SDL2Application.h"

void CreateApplication();

int wmain()
{
    wWinMain(GetModuleHandle(0), 0, 0, SW_SHOW);
}

//Keep reference for parameter names
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	CreateApplication();
	return 0;
}

void CreateApplication()
{
	//Spacebar:     start/stop simulation
	//Enter:        show/hide the grid
	//Backspace:    clear the grid

	//Create an application, you can give it the size of a cell
	Application* app{ new SDL2Application{15} };

	//Run the application
	app->Run();

	delete app;
}