#include <iostream>
#include <vld.h>

#include "CGOLApplication.h"
#include "OpenGLRenderer.h"
#include "SDL2Renderer.h"

int main()
{
    Renderer* pRenderer = new SDL2Renderer{ "Conway's Game Of Life", 1920, 1080 };
    CGOLApplication app{ pRenderer };

    app.Run();

    return 0;
}