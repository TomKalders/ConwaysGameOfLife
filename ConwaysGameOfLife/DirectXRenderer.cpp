#include "DirectXRenderer.h"

#include <iostream>

DirectXRenderer::DirectXRenderer(HINSTANCE hInstance, const std::string& windowName, int width, int height)
	: m_Instance(hInstance)
	, m_WindowTitle(windowName)
	, m_WindowTitleWide(std::wstring{windowName.begin(), windowName.end()})
	, m_ClassTitle(L"MainWindowClass")
	, m_Width(width)
	, m_Height(height)
{
}

bool DirectXRenderer::Initialize(Grid*)
{
	RegisterWindowClass();

	m_Handle = CreateWindowEx(
		0,
		m_ClassTitle.c_str(),
		m_WindowTitleWide.c_str(),
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		0,
		0,
		m_Width,
		m_Height,
		NULL,
		NULL,
		m_Instance,
		nullptr
	);

	if (m_Handle == NULL)
	{
		std::cout << GetLastError() << ": failed to create window in DirectXApplication" << std::endl;
		return false;
	}

	ShowWindow(m_Handle, SW_SHOW);
	SetForegroundWindow(m_Handle);
	SetFocus(m_Handle);

	return true;
}

void DirectXRenderer::Render() const
{
}

void DirectXRenderer::Cleanup()
{
	if (m_Handle != NULL)
	{
		UnregisterClass(m_ClassTitle.c_str(), m_Instance);
		DestroyWindow(m_Handle);
	}
}

int DirectXRenderer::GetWindowWidth() const
{
	return m_Width;
}

int DirectXRenderer::GetWindowHeight() const
{
	return m_Height;
}

void DirectXRenderer::ToggleGrid()
{
}

void DirectXRenderer::RegisterWindowClass()
{
	WNDCLASSEX wc{0};
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_Instance;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_WindowTitleWide.c_str();
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);
}
