#include "DirectXRenderer.h"

#include <memory>
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
    //RegisterWindowClass();

    //m_Handle = CreateWindowEx(
    //	0,
    //	m_ClassTitle.c_str(),
    //	m_WindowTitleWide.c_str(),
    //	WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
    //	0,
    //	0,
    //	m_Width,
    //	m_Height,
    //	NULL,
    //	NULL,
    //	m_Instance,
    //	nullptr
    //);

    //if (m_Handle == NULL)
    //{
    //	std::cout << GetLastError() << ": failed to create window in DirectXApplication" << std::endl;
    //	return false;
    //}

    //ShowWindow(m_Handle, SW_SHOW);
    //SetForegroundWindow(m_Handle);
    //SetFocus(m_Handle);

    // Instantiate the window manager class.
// Window resources are dealt with here.

    if (m_Instance == NULL)
        m_Instance = (HINSTANCE)GetModuleHandle(NULL);

    HICON hIcon = NULL;
    WCHAR szExePath[MAX_PATH];
    GetModuleFileName(NULL, szExePath, MAX_PATH);

    // If the icon is NULL, then use the first one found in the exe
    if (hIcon == NULL)
        hIcon = ExtractIcon(m_Instance, szExePath, 0);

    // Register the windows class
    WNDCLASS wndClass;
    wndClass.style = CS_DBLCLKS;
    wndClass.lpfnWndProc = DefWindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = m_Instance;
    wndClass.hIcon = hIcon;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = m_WindowTitleWide.c_str();

    if (!RegisterClass(&wndClass))
    {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            return HRESULT_FROM_WIN32(dwError);
    }

    m_Rect;
    int x = CW_USEDEFAULT;
    int y = CW_USEDEFAULT;

    // No menu in this example.
    m_Menu = NULL;

    // This example uses a non-resizable 640 by 480 viewport for simplicity.
    int nDefaultWidth = 640;
    int nDefaultHeight = 480;
    SetRect(&m_Rect, 0, 0, nDefaultWidth, nDefaultHeight);
    AdjustWindowRect(
        &m_Rect,
        WS_OVERLAPPEDWINDOW,
        (m_Menu != NULL) ? true : false
    );

    // Create the window for our viewport.
    m_Handle = CreateWindow(
        m_WindowTitleWide.c_str(),
        L"Cube11",
        WS_OVERLAPPEDWINDOW,
        x, y,
        (m_Rect.right - m_Rect.left), (m_Rect.bottom - m_Rect.top),
        0,
        m_Menu,
        m_Instance,
        0
    );

    if (m_Handle == NULL)
    {
        DWORD dwError = GetLastError();
        return HRESULT_FROM_WIN32(dwError);
    }

    return S_OK;
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

HWND DirectXRenderer::GetHandle()
{
    return m_Handle;
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
