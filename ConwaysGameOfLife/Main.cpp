#include <iostream>
#include <vld.h>

#include "CGOLApplication.h"
#include "SDL2Renderer.h"

int main()
{
    //Spacebar:     start/stop simulation
    //Enter:        show/hide the grid
    //Backspace:    clear the grid
    
    //Create a renderer to create a window and draw the grid
    Renderer* pRenderer = new SDL2Renderer{ "Conway's Game Of Life", 1280, 960 };

    //Create an application, you can give it the size of a cell
    CGOLApplication app{ pRenderer, 15 };

    //Run the application
    app.Run();

    return 0;
}