#include <iostream>
#include <vld.h>
#include <GLFW/glfw3.h>
#include "SDL.h"
#undef main

#include "CGOLApplication.h"
#include "OpenGLRenderer.h"

int main()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    Renderer* pRenderer = new OpenGLRenderer{ "Conway's Game Of Life", 640, 480 };

    SDL_Window* window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 400, SDL_WINDOW_HIDDEN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 0);

    CGOLApplication app{ pRenderer };
    app.Run();



    //bool running = true;
    //while (running)
    //{
    //    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    //    SDL_RenderClear(renderer);
    //    SDL_RenderPresent(renderer);

    //    SDL_Event e;
    //    while (SDL_PollEvent(&e))
    //    {
    //        switch (e.type)
    //        {
    //        case SDL_MOUSEBUTTONDOWN:
    //            std::cout << "Mouse down" << std::endl;
    //        case SDL_QUIT:
    //            running = false;
    //        }
    //    }
    //}


    return 0;
}