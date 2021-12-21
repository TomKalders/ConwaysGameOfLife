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
        bool useOtter = true;

        Mesh* mesh = nullptr;
        if (!useOtter)
	        mesh = new Mesh{ m_pDirectXRenderer->GetDevice(), "Resources/Models/Torus.obj" };
        else
			mesh = new Mesh{ m_pDirectXRenderer->GetDevice(), "Resources/Models/Sea otter.obj" };

        m_pDirectXRenderer->AddMesh(mesh);
        //auto vertexBuffer = mesh->GetVertexBuffer();
        //if (vertexBuffer.size() > 0)
        //{
        //    vertexBuffer[0].color1 = { 1, 0, 0 };
        //    mesh->SetVertexBuffer(m_pDirectXRenderer->GetDeviceContext(), vertexBuffer);
        //}
    }

	return true;
}

void DirectXApplication::PostInitialize()
{
    GetMeshNeighbours();
    PerspectiveCamera* pCamera = m_pDirectXRenderer->GetCamera();
    pCamera->SetMovementSpeed(5.f);
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

    for (Mesh* const mesh : meshes)
    {
	    if (mesh)
	    {
            mesh->UpdateMesh(m_pDirectXRenderer->GetDeviceContext(), deltaTime);
	    }
    }
}

void DirectXApplication::Cleanup()
{
	m_pRenderer->Cleanup();
    delete m_pRenderer;
}

void DirectXApplication::GetMeshNeighbours()
{
    const std::vector<Mesh*>& meshes = m_pDirectXRenderer->GetMeshes();

    for (Mesh* const mesh : meshes)
    {
        const std::vector<uint32_t>& indexBuffer = mesh->GetIndexBuffer();
        std::vector<VertexInput> vertexBuffer = mesh->GetVertexBuffer();

        for (uint32_t i{ 0 }; i < indexBuffer.size(); i++)
        {
            std::vector<uint32_t>::const_iterator it = std::find(indexBuffer.begin(), indexBuffer.end(), i);
            while (it != indexBuffer.end())
            {
                uint32_t index = uint32_t(it - indexBuffer.begin());
                int modulo = index % 3;
                if (modulo == 0)
                {
                    if (it + 1 != indexBuffer.end())
                        vertexBuffer[i].neighbourIndices.insert(*(it + 1));
                    if (it + 2 != indexBuffer.end())
                        vertexBuffer[i].neighbourIndices.insert(*(it + 2));
                }
                else if (modulo == 1)
                {
                    if (it - 1 != indexBuffer.end())
                        vertexBuffer[i].neighbourIndices.insert(*(it - 1));
                    if (it + 1 != indexBuffer.end())
                        vertexBuffer[i].neighbourIndices.insert(*(it + 1));
                }
                else
                {
                    if (it - 1 != indexBuffer.end())
                        vertexBuffer[i].neighbourIndices.insert(*(it - 1));
                    if (it - 2 != indexBuffer.end())
                        vertexBuffer[i].neighbourIndices.insert(*(it - 2));
                }

                it++;
                it = std::find(it, indexBuffer.end(), i);
            }
        }

        mesh->SetVertexBuffer(m_pDirectXRenderer->GetDeviceContext(), vertexBuffer);
    }
}
