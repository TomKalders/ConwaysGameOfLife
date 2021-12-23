#include "DirectXApplication.h"
#include "DirectXRenderer.h"

#include "SDL.h"

#include <chrono>
#include <algorithm>
//Undifine max macro becuase it interferes with the std::max() algorithm.
#undef max


DirectXApplication::DirectXApplication(HINSTANCE hInstance)
	: Application(new DirectXRenderer{ hInstance, {"Heart Pulse Simulation: DirectX"}, 1820, 960 })
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
        //vertex 20093 is a vertex that can pbe pulsed for the body

        //enum MeshType
        //{
	       // Torus,
        //    Otter,
        //    Heart
        //};
        //MeshType meshType = MeshType::Heart;
        //Mesh* mesh = nullptr;

        //switch (meshType)
        //{
        //case Torus:
        //    mesh = new Mesh{ m_pDirectXRenderer->GetDevice(), "Resources/Models/Torus.obj" };
        //    break;
        //case Otter:	
        //    mesh = new Mesh{ m_pDirectXRenderer->GetDevice(), "Resources/Models/Sea otter.obj" };
        //    break;
        //case Heart:
        //    mesh = new Mesh{ m_pDirectXRenderer->GetDevice(), "Resources/Models/Heart.obj" };
        //    break;
        //default:
        //    mesh = new Mesh{ m_pDirectXRenderer->GetDevice(), "Resources/Models/Torus.obj" };
        //    break;
        //}

        //m_pDirectXRenderer->AddMesh(mesh);
    }

	return true;
}

void DirectXApplication::PostInitialize()
{
    PerspectiveCamera* pCamera = m_pDirectXRenderer->GetCamera();
    pCamera->SetMovementSpeed(2.f);
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
    if (state[SDL_SCANCODE_UP])
    {
        glm::fvec3 upVector{ glm::fvec3{0, 1, 0} * pCamera->GetMovementSpeed() * m_DeltaTime };
        pCamera->Translate(upVector);
    }
    if (state[SDL_SCANCODE_DOWN])
    {
        glm::fvec3 upVector{ glm::fvec3{0, 1, 0} *pCamera->GetMovementSpeed() * m_DeltaTime };
        pCamera->Translate(-upVector);
    }


    SDL_Event e;

    while (SDL_PollEvent(&e))
    {
        if (ImGui::GetIO().WantCaptureKeyboard)
        {
            ImGui_ImplSDL2_ProcessEvent(&e);
        }
        //else
        //{
        //    switch (e.type)
        //    {
        //    case SDL_QUIT:
        //        //If the close button on the window is hit, exit the application
        //        DirectXApplication::QuitApplication();
        //        break;
        //    default:
        //        break;
        //    }
        //}

        if (e.type == SDL_QUIT)
            DirectXApplication::QuitApplication();
    }
}

void DirectXApplication::Update(float deltaTime)
{
    const std::vector<Mesh*>& meshes = m_pDirectXRenderer->GetMeshes();

    for (Mesh* const mesh : meshes)
    {
	    if (mesh)
	    {
            if (m_pDirectXRenderer->UseVersionOne())
            {
                mesh->UpdateMesh(m_pDirectXRenderer->GetDeviceContext(), deltaTime);
            }
            else
            {
                mesh->UpdateMeshV2(m_pDirectXRenderer->GetDeviceContext(), deltaTime);
            }
	    }
    }
}

void DirectXApplication::Cleanup()
{
	m_pRenderer->Cleanup();
    delete m_pRenderer;
}
