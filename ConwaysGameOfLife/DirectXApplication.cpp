#include "DirectXApplication.h"
#include "DirectXRenderer.h"

DirectXApplication::DirectXApplication(HINSTANCE hInstance)
	: Application(new DirectXRenderer{ hInstance, {"Conway's Game of Life: DirectX"}, 1820, 960 })
	, m_Width(1820)
	, m_Height(960)
{
	UNREFERENCED_PARAMETER(hInstance);
}

void DirectXApplication::Initialize()
{
}

void DirectXApplication::HandleInput()
{
}

void DirectXApplication::Update(float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);
}

void DirectXApplication::Cleanup()
{
}
