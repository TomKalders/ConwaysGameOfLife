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

void DirectXApplication::Update(float deltaTime)
{
    const std::vector<Mesh*>& meshes = m_pDirectXRenderer->GetMeshes();

	for (Mesh* mesh : meshes)
	{
  //      std::vector<VertexInput> newVertices;
		//for (const VertexInput& vertex : mesh->GetVertexBuffer())
		//{
  //          VertexInput newVertex{ vertex };
  //          newVertex.power += deltaTime;
		//	if (newVertex.power > 255.f)
		//	{
  //              newVertex.power = 0.f;
		//	}
  //          newVertices.push_back(newVertex);
		//}
        //mesh->SetVertexBuffer(m_pDirectXRenderer->GetDeviceContext(), newVertices);

        std::vector<float> newPowerValues{};
        const std::vector<float>& powerValues = mesh->GetPowerBuffer();
        newPowerValues.reserve(powerValues.size());

        for (int i{}; i < powerValues.size(); i++)
        {
            float value = powerValues[i] + deltaTime;
            if (value > 255.f)
                value = 0;

            newPowerValues.push_back(value);
        }

        mesh->SetPowerBuffer(m_pDirectXRenderer->GetDeviceContext(), newPowerValues);
	}
}

void DirectXApplication::Cleanup()
{
	m_pRenderer->Cleanup();
    delete m_pRenderer;
}
