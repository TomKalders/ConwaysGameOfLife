#include "DirectXApplication.h"
#include "DirectXRenderer.h"
#include <chrono>
#include "SDL.h"

DirectXApplication::DirectXApplication(HINSTANCE hInstance)
	: Application(new DirectXRenderer{ hInstance, {"Conway's Game of Life: DirectX"}, 1820, 960 })
{
	UNREFERENCED_PARAMETER(hInstance);
}

void DirectXApplication::Run()
{
    if (!Initialize())
    {
        throw std::exception("Application not initialized");
    }

    m_pDirectXRenderer = static_cast<DirectXRenderer*>(m_pRenderer);
    if (!m_pDirectXRenderer)
    {
        throw std::exception{ "Unsupported renderer for this application" };
    }

    HWND handle = m_pDirectXRenderer->GetHandle();
    if (!IsWindowVisible(handle))
        ShowWindow(handle, SW_SHOW);

    //// The render loop is controlled here.
    //bool bGotMsg;
    //MSG  msg;
    //msg.message = WM_NULL;
    //PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	//Calculate DeltaTime
    auto timeLastFrame = std::chrono::high_resolution_clock::now();
	
    while (/*WM_QUIT != msg.message &&*/ m_IsRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        m_DeltaTime = std::chrono::duration<float>(currentTime - timeLastFrame).count();
    	
        // Process window events.
        // Use PeekMessage() so we can use idle time to render the scene. 
        //bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

        //if (bGotMsg)
        //{
        //    // Translate and dispatch the message
        //    TranslateMessage(&msg);
        //    DispatchMessage(&msg);
        //}
        //else
        //{
            HandleInput();
            Update(m_DeltaTime);
            m_pRenderer->Render();
        //}

        timeLastFrame = currentTime;
    }

    Cleanup();
}

bool DirectXApplication::Initialize()
{
    //SDL_Init(SDL_INIT_EVERYTHING);

	if (!m_pRenderer->Initialize(nullptr))
		return false;

	return true;
}

void DirectXApplication::HandleInput()
{
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    PerspectiveCamera* pCamera = m_pDirectXRenderer->GetCamera();

	if (state[SDL_SCANCODE_W])
	{
        glm::fvec3 forwardVector{ pCamera->GetForwardVector() * pCamera->GetMovementSpeed() * m_DeltaTime };
        pCamera->Translate(forwardVector);
	}
    if (state[SDL_SCANCODE_S])
    {
        glm::fvec3 forwardVector{ pCamera->GetForwardVector() * pCamera->GetMovementSpeed() * m_DeltaTime };
        pCamera->Translate(-forwardVector);
    }
    if (state[SDL_SCANCODE_A])
    {
        glm::fvec3 rightVector{ pCamera->GetRightVector() * pCamera->GetMovementSpeed() * m_DeltaTime };
        pCamera->Translate(-rightVector);
    }
    if (state[SDL_SCANCODE_D])
    {
        glm::fvec3 rightVector{ pCamera->GetRightVector() * pCamera->GetMovementSpeed() * m_DeltaTime };
        pCamera->Translate(rightVector);
    }

    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        switch (e.type)
        {
        case SDL_QUIT:
            //If the close button on the window is hit, exit the application
            DirectXApplication::QuitApplication();
            break;
        }
    }
}

void DirectXApplication::Update(float)
{
}

void DirectXApplication::Cleanup()
{
	m_pRenderer->Cleanup();
    delete m_pRenderer;
}
