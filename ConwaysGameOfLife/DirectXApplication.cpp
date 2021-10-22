#include "DirectXApplication.h"
#include "DirectXRenderer.h"
#include <chrono>

DirectXApplication::DirectXApplication(HINSTANCE hInstance)
	: Application(new DirectXRenderer{ hInstance, {"Conway's Game of Life: DirectX"}, 1820, 960 })
{
	UNREFERENCED_PARAMETER(hInstance);
}

void DirectXApplication::Run()
{
    Initialize();
	
    DirectXRenderer* pCastedRenderer = static_cast<DirectXRenderer*>(m_pRenderer);
    if (!pCastedRenderer)
    {
        throw std::exception{ "Unsupported renderer for this application" };
    }

    HWND handle = pCastedRenderer->GetHandle();
    if (!IsWindowVisible(handle))
        ShowWindow(handle, SW_SHOW);

    // The render loop is controlled here.
    bool bGotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	//Calculate DeltaTime
    auto timeLastFrame = std::chrono::high_resolution_clock::now();
	
    while (WM_QUIT != msg.message && m_IsRunning)
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(currentTime - timeLastFrame).count();
    	
        // Process window events.
        // Use PeekMessage() so we can use idle time to render the scene. 
        bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

        if (bGotMsg)
        {
            // Translate and dispatch the message
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            HandleInput();
            Update(deltaTime);
            m_pRenderer->Render();
        }

        timeLastFrame = currentTime;
    }
}

bool DirectXApplication::Initialize()
{
	if (!m_pRenderer->Initialize(nullptr))
		return false;

	return true;
}

void DirectXApplication::HandleInput()
{
}

void DirectXApplication::Update(float)
{
}

void DirectXApplication::Cleanup()
{
	m_pRenderer->Cleanup();
}
