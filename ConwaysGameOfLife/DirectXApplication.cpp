#include "DirectXApplication.h"
#include "DirectXRenderer.h"

DirectXApplication::DirectXApplication(HINSTANCE hInstance)
	: Application(new DirectXRenderer{ hInstance, {"Conway's Game of Life: DirectX"}, 1820, 960 })
{
	UNREFERENCED_PARAMETER(hInstance);
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
