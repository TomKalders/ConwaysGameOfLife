#include <windows.h>
#include <iostream>
#include <vld.h>

#include "SDL2Application.h"
#include "DirectXApplication.h"

void CreateApplication(HINSTANCE hInstance);

int wmain()
{
    wWinMain(GetModuleHandle(0), 0, 0, SW_SHOW);
}

//Keep reference for parameter names
//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int)
{
	CreateApplication(hInstance);
	return 0;
}

void CreateApplication(HINSTANCE)
{
	//Spacebar:     start/stop simulation
	//Enter:        show/hide the grid
	//Backspace:    clear the grid

	//Create an application, you can give it the size of a cell
	Application* app{ new SDL2Application{15} };
	//Application* app{ new DirectXApplication{hInstance} };
	
	//Run the application
	app->Run();

	delete app;
}