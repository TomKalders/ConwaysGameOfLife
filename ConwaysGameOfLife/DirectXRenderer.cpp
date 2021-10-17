#include "DirectXRenderer.h"

DirectXRenderer::DirectXRenderer(HINSTANCE hInstance, const std::string& windowName, int width, int height)
{
	UNREFERENCED_PARAMETER(hInstance);
	UNREFERENCED_PARAMETER(windowName);
	UNREFERENCED_PARAMETER(width);
	UNREFERENCED_PARAMETER(height);
}

void DirectXRenderer::Initialize(Grid*)
{
}

void DirectXRenderer::Render() const
{
}

void DirectXRenderer::Cleanup()
{
}

int DirectXRenderer::GetWindowWidth() const
{
	return 0;
}

int DirectXRenderer::GetWindowHeight() const
{
	return 0;
}

void DirectXRenderer::ToggleGrid()
{
}
