#include "DirectXRenderer.h"

#include <memory>
#include <iostream>

HINSTANCE DirectXRenderer::m_Instance = NULL;
std::wstring DirectXRenderer::m_ClassTitle = L"DefaultClass";

DirectXRenderer::DirectXRenderer(HINSTANCE hInstance, const std::string& windowName, int width, int height)
	//: m_Instance(hInstance)
	: m_WindowTitle(windowName)
	, m_WindowTitleWide(std::wstring{windowName.begin(), windowName.end()})
	//, m_ClassTitle(L"MainWindowClass")
	, m_Width(width)
	, m_Height(height)
{
    m_Instance = hInstance;
}

bool DirectXRenderer::Initialize(Grid*)
{
    //RegisterWindowClass();

    if (CreateHandle() != S_OK)
        return false;

    if (InitializeDirectX() != S_OK)
        return false;
	
    return true;
}

void DirectXRenderer::Render() const
{
    //Clear buffer
    glm::vec3 clearColor{ 0.2f, 0.2f, 0.6f };
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    //Render
	//...

    //Present
    m_pSwapChain->Present(0, 0);
}

void DirectXRenderer::Cleanup()
{
    if (m_pRenderTargetView)
    {
        m_pRenderTargetView->Release();
        //delete m_pRenderTargetView;
    }

    if (m_pRenderTargetBuffer)
    {
        m_pRenderTargetBuffer->Release();
        //delete m_pRenderTargetBuffer;
    }

    if (m_pDepthStencilView)
    {
        m_pDepthStencilView->Release();
    }
    if (m_pDepthStencilBuffer)
    {
        m_pDepthStencilBuffer->Release();
	    
    }

    if (m_pSwapChain)
    {
        m_pSwapChain->Release();
	    
    }

    if (m_pDeviceContext)
    {
        m_pDeviceContext->ClearState();
        m_pDeviceContext->Flush();
        m_pDeviceContext->Release();
    }

    if (m_pDevice)
    {
        m_pDevice->Release();
	    
    }

    if (m_pDXGIFactory)
    {
        m_pDXGIFactory->Release();
	    
    }
	
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

HRESULT DirectXRenderer::InitializeDirectX()
{
        //Create Device and Device Context, using hardware acceleration
        D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
        uint32_t createDeviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT result = D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE, 0, createDeviceFlags, 0, 0, D3D11_SDK_VERSION, &m_pDevice, &featureLevel, &m_pDeviceContext);
        if (FAILED(result))
            return result;

        //Create DXGI Factory to create SwapChain based on hardware
        result = CreateDXGIFactory(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&m_pDXGIFactory));
        if (FAILED(result))
            return result;

        //Create the swapchain discriptor
        DXGI_SWAP_CHAIN_DESC swapChainDesc{};
        swapChainDesc.BufferDesc.Width = m_Width;
        swapChainDesc.BufferDesc.Height = m_Height;
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 1;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 60;
        swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
        swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = 1;
        swapChainDesc.Windowed = true;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags = 0;

        //Get the handle HWND from the sdl backbuffer
        //SDL_SysWMinfo sysWMInfo{};
        //SDL_VERSION(&sysWMInfo.version);
        //SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
        swapChainDesc.OutputWindow = m_Handle;

        //Create swapchain and hook it into the handle of the SDL window
        result = m_pDXGIFactory->CreateSwapChain(m_pDevice, &swapChainDesc, &m_pSwapChain);
        if (FAILED(result))
            return result;

        //Create the depth/stensil buffer and view
        D3D11_TEXTURE2D_DESC depthStensilDesc{};
        depthStensilDesc.Width = m_Width;
        depthStensilDesc.Height = m_Height;
        depthStensilDesc.MipLevels = 1;
        depthStensilDesc.ArraySize = 1;
        depthStensilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStensilDesc.SampleDesc.Count = 1;
        depthStensilDesc.SampleDesc.Quality = 0;
        depthStensilDesc.Usage = D3D11_USAGE_DEFAULT;
        depthStensilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthStensilDesc.CPUAccessFlags = 0;
        depthStensilDesc.MiscFlags = 0;

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
        depthStencilViewDesc.Format = depthStensilDesc.Format;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        result = m_pDevice->CreateTexture2D(&depthStensilDesc, 0, &m_pDepthStencilBuffer);
        if (FAILED(result))
            return result;

        result = m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView);
        if (FAILED(result))
            return result;

        //Create the render target view
        result = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_pRenderTargetBuffer));
        if (FAILED(result))
            return result;

        result = m_pDevice->CreateRenderTargetView(m_pRenderTargetBuffer, 0, &m_pRenderTargetView);
        if (FAILED(result))
            return result;

        //Bind the views to the output Merger Stage
        m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

        //Set the viewport
        D3D11_VIEWPORT viewPort{};
        viewPort.Width = static_cast<float>(m_Width);
        viewPort.Height = static_cast<float>(m_Height);
        viewPort.TopLeftX = 0.f;
        viewPort.TopLeftY = 0.f;
        viewPort.MinDepth = 0.f;
        viewPort.MaxDepth = 1.f;
        m_pDeviceContext->RSSetViewports(1, &viewPort);

        return HRESULT{ S_OK };
    }

HRESULT DirectXRenderer::CreateHandle()
{
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
    wndClass.lpfnWndProc = WindowProcedure;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = m_Instance;
    wndClass.hIcon = hIcon;
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = m_ClassTitle.c_str();

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
    //int nDefaultWidth = 640;
    //int nDefaultHeight = 480;
    SetRect(&m_Rect, 0, 0, m_Width, m_Height);
    AdjustWindowRect(
        &m_Rect,
        WS_OVERLAPPEDWINDOW,
        (m_Menu != NULL) ? true : false
    );

    // Create the window for our viewport.
    m_Handle = CreateWindow(
        m_ClassTitle.c_str(),
        m_WindowTitleWide.c_str(),
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

LRESULT CALLBACK DirectXRenderer::WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
    {
        HMENU hMenu;
        hMenu = GetMenu(hWnd);
        if (hMenu != NULL)
        {
            DestroyMenu(hMenu);
        }
        DestroyWindow(hWnd);
        UnregisterClass(
            m_ClassTitle.c_str(),
            m_Instance
        );
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//void DirectXRenderer::RegisterWindowClass()
//{
//	WNDCLASSEX wc{0};
//	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
//	wc.lpfnWndProc = DefWindowProc;
//	wc.cbClsExtra = 0;
//	wc.cbWndExtra = 0;
//	wc.hInstance = m_Instance;
//	wc.hIcon = NULL;
//	wc.hIconSm = NULL;
//	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
//	wc.hbrBackground = NULL;
//	wc.lpszMenuName = NULL;
//	wc.lpszClassName = m_WindowTitleWide.c_str();
//	wc.cbSize = sizeof(WNDCLASSEX);
//
//	RegisterClassEx(&wc);
//}
