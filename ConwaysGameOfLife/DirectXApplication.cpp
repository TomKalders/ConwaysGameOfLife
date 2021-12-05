#include "DirectXApplication.h"
#include "DirectXRenderer.h"
#include <chrono>
#include "SDL.h"

DirectXApplication::DirectXApplication(HINSTANCE hInstance)
	: Application(new DirectXRenderer{ hInstance, {"Conway's Game of Life: DirectX"}, 1820, 960 })
{
	UNREFERENCED_PARAMETER(hInstance);
}

bool DirectXApplication::Initialize()
{
	if (!m_pRenderer->Initialize(nullptr))
		return false;

    m_pDirectXRenderer = static_cast<DirectXRenderer*>(m_pRenderer);
    if (!m_pDirectXRenderer)
    {
        throw std::exception{ "Unsupported renderer for this application" };
    }

    if (m_pDirectXRenderer)
    {
        Mesh* mesh = new Mesh{ m_pDirectXRenderer->GetDevice(), "Resources/Models/Torus.obj" };
        m_pDirectXRenderer->AddMesh(mesh);
    }

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
    if (state[SDL_SCANCODE_E])
    {
        pCamera->RotateYaw(pCamera->GetRotationSpeed() * m_DeltaTime);
    }

    if (state[SDL_SCANCODE_Q])
    {
        pCamera->RotateYaw(-pCamera->GetRotationSpeed() * m_DeltaTime);
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
