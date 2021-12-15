#include "DirectXRenderer.h"
#include "Mesh.h"

#include <memory>
#include <iostream>

#include "SDL_syswm.h"
#include "SDL_surface.h"

#pragma warning(push)
#pragma warning(disable:4616)
#pragma warning(disable:4201)
#include <gtc/type_ptr.hpp>
#pragma warning(pop)

HINSTANCE DirectXRenderer::m_Instance = NULL;
std::wstring DirectXRenderer::m_ClassTitle = L"DefaultClass";

DirectXRenderer::DirectXRenderer(HINSTANCE hInstance, const std::string& windowName, int width, int height)
	: m_pCamera(new PerspectiveCamera{{0, 0, 10}, {0, 0, 1}, float(width)/float(height)})
	, m_pWindow(nullptr)
	, m_WindowTitle(windowName)
	, m_WindowTitleWide(std::wstring{windowName.begin(), windowName.end()})
	, m_Width(width)
	, m_Height(height)
    , m_pInputBuffer(new char[16])
{
    m_Instance = hInstance;
}

bool DirectXRenderer::Initialize(Grid*)
{
    //RegisterWindowClass();

    //if (CreateHandle() != S_OK)
    //    return false;

    if (!InitializeSDLWindow())
        return false;

    if (InitializeDirectX() != S_OK)
        return false;

    if (!InitializeImGui())
        return false;

    return true;
}

void DirectXRenderer::Render()
{
    if (!m_pDeviceContext && !m_pSwapChain)
        return;

    //Clear buffer
    glm::vec3 clearColor{ 0.2f, 0.2f, 0.6f };
    m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, &clearColor.r);
    m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

    //Render

    if (!m_pMeshes.empty())
    {
        RenderMeshes();
    }

    RenderImGui();

    //Present
    m_pSwapChain->Present(0, 0);
}

void DirectXRenderer::Cleanup()
{
    if (m_pRenderTargetView)
    {
        m_pRenderTargetView->Release();
    }

    if (m_pRenderTargetBuffer)
    {
        m_pRenderTargetBuffer->Release();
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

    if (m_pCamera)
        delete m_pCamera;

    for (Mesh* mesh : m_pMeshes)
    {
        if (mesh)
        {
            delete mesh;
            mesh = nullptr;
        }
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    if (m_pWindow)
    {
        SDL_DestroyWindow(m_pWindow);
		SDL_Quit();
    }

    if (m_pInputBuffer)
    {
        delete m_pInputBuffer;
    	m_pInputBuffer = nullptr;
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

HWND DirectXRenderer::GetHandle() const
{
	return m_Handle;
}

PerspectiveCamera* DirectXRenderer::GetCamera() const
{
    return m_pCamera;
}

ID3D11Device* const DirectXRenderer::GetDevice() const
{
    return m_pDevice;
}

ID3D11DeviceContext* const DirectXRenderer::GetDeviceContext() const
{
    return m_pDeviceContext;
}

void DirectXRenderer::AddMesh(Mesh* pMesh)
{
    if (pMesh)
    {
        if (!pMesh->GetVertexBuffer().empty())
            m_pMeshes.push_back(pMesh);
        else
            std::cout << "Trying to add mesh without vertices!" << std::endl;
    }
}

void DirectXRenderer::RemoveMesh(Mesh* pmesh)
{
    std::vector<Mesh*>::const_iterator it = std::find(m_pMeshes.begin(), m_pMeshes.end(), pmesh);
    if (it != m_pMeshes.end())
    {
        m_pMeshes.erase(it);
    }
}

const std::vector<Mesh*>& DirectXRenderer::GetMeshes() const
{
    return m_pMeshes;
}

bool DirectXRenderer::InitializeSDLWindow()
{
    SDL_Init(SDL_INIT_VIDEO);

    m_pWindow = SDL_CreateWindow(
        m_WindowTitle.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        m_Width, m_Height, 0);

    return m_pWindow != nullptr;
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
        SDL_SysWMinfo sysWMInfo{};
        SDL_VERSION(&sysWMInfo.version);
        SDL_GetWindowWMInfo(m_pWindow, &sysWMInfo);
        swapChainDesc.OutputWindow = sysWMInfo.info.win.window;
        //swapChainDesc.OutputWindow = m_Handle;

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

bool DirectXRenderer::InitializeImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    if (!ImGui_ImplSDL2_InitForD3D(m_pWindow))
        return false;

    if (!ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext))
        return false;

    return true;
}

void DirectXRenderer::RenderImGui()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplSDL2_NewFrame(m_pWindow);
    ImGui::NewFrame();

    ImGui::Begin("Data");
    ImGui::DragFloat4("Lookat[0]", glm::value_ptr(m_pCamera->GetLookAt()[0]));
    ImGui::DragFloat4("Lookat[1]", glm::value_ptr(m_pCamera->GetLookAt()[1]));
    ImGui::DragFloat4("Lookat[2]", glm::value_ptr(m_pCamera->GetLookAt()[2]));
    ImGui::DragFloat4("Lookat[3]", glm::value_ptr(m_pCamera->GetLookAt()[3]));
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::DragFloat4("Projection[0]", glm::value_ptr(m_pCamera->GetProjectionMatrix()[0]));
    ImGui::DragFloat4("Projection[1]", glm::value_ptr(m_pCamera->GetProjectionMatrix()[1]));
    ImGui::DragFloat4("Projection[2]", glm::value_ptr(m_pCamera->GetProjectionMatrix()[2]));
    ImGui::DragFloat4("Projection[3]", glm::value_ptr(m_pCamera->GetProjectionMatrix()[3]));
    ImGui::Spacing();
    ImGui::Spacing();
    ImGui::Spacing();
    float power = m_pMeshes[0]->GetPowerBuffer()[0];
    ImGui::DragFloat("Power", &power);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void DirectXRenderer::RenderMeshes() const
{
    const glm::mat4 viewMatrix = m_pCamera->GetViewMatrix();
    glm::mat4 inverseViewMatrix = glm::inverse(viewMatrix);

    //Credit to Pepijn Langeraert for finding this issue
    const glm::mat4 projectionMatrix = glm::transpose(m_pCamera->GetProjectionMatrix());

    inverseViewMatrix = glm::transpose(inverseViewMatrix);
    float* inverseView = (float*)glm::value_ptr(inverseViewMatrix);

    for (Mesh* const mesh : m_pMeshes)
    {
        glm::mat4 worldViewProjectionMatrix = projectionMatrix * viewMatrix * mesh->GetWorldMatrix();
        worldViewProjectionMatrix = glm::transpose(worldViewProjectionMatrix);
        float* worldViewProjection = (float*)glm::value_ptr(worldViewProjectionMatrix);

        mesh->Render(m_pDeviceContext, worldViewProjection, inverseView);
    }
}





//Example vertex and index buffer for reference

//const static std::vector<VertexInput> vertices =
//{
//    VertexInput{{-0.5f,0.5f,-0.5f},   {255, 255, 255}},
//    VertexInput{{-0.5f,-0.5f,-0.5f},  {255, 255, 255}},
//    VertexInput{{0.5f,-0.5f,-0.5f},   {255, 255, 255}},
//    VertexInput{{0.5f,0.5f,-0.5f},    {255, 255, 255}},
//                                                       
//    VertexInput{{-0.5f,0.5f,0.5f},    {255, 255, 255}},
//    VertexInput{{-0.5f,-0.5f,0.5f},   {255, 255, 255}},
//    VertexInput{{0.5f,-0.5f,0.5f},    {255, 255, 255}},
//    VertexInput{{0.5f,0.5f,0.5f},     {255, 255, 255}},
//                                                       
//    VertexInput{{0.5f,0.5f,-0.5f},    {255, 255, 255}},
//    VertexInput{{0.5f,-0.5f,-0.5f},   {255, 255, 255}},
//    VertexInput{{0.5f,-0.5f,0.5f},    {255, 255, 255}},
//    VertexInput{{0.5f,0.5f,0.5f},     {255, 255, 255}},
//                                                       
//    VertexInput{{-0.5f,0.5f,-0.5f},   {255, 255, 255}},
//    VertexInput{{-0.5f,-0.5f,-0.5f},  {255, 255, 255}},
//    VertexInput{{-0.5f,-0.5f,0.5f},   {255, 255, 255}},
//    VertexInput{{-0.5f,0.5f,0.5f},    {255, 255, 255}},
//                                                       
//    VertexInput{{-0.5f,0.5f,0.5f},    {255, 255, 255}},
//    VertexInput{{-0.5f,0.5f,-0.5f},   {255, 255, 255}},
//    VertexInput{{0.5f,0.5f,-0.5f},    {255, 255, 255}},
//    VertexInput{{0.5f,0.5f,0.5f},     {255, 255, 255}},
//                                                       
//    VertexInput{{-0.5f,-0.5f,0.5f},   {255, 255, 255}},
//    VertexInput{{-0.5f,-0.5f,-0.5f},  {255, 255, 255}},
//    VertexInput{{0.5f,-0.5f,-0.5f},   {255, 255, 255}},
//    VertexInput{{0.5f,-0.5f,0.5f},    {255, 255, 255}}

//     //VertexInput{{-1, -1, -1},  /*{0, 0, 1} , */{145, 145, 145} },
//     //VertexInput{{1, -1, -1} ,  /*{1, 0, 0} , */{145, 145, 145} },
//     //VertexInput{{1, 1, -1}  ,  /*{0, 0, -1}, */{145, 145, 145} },
//     //VertexInput{{-1, 1, -1} ,  /*{-1, 0, 0}, */{145, 145, 145} },
//     //VertexInput{{-1, -1, 1} ,  /*{0, 1, 0} , */{145, 145, 145} },
//     //VertexInput{{1, -1, 1}  ,  /*{0, -1, 0}, */{145, 145, 145} },
//     //VertexInput{{1, 1, 1}   ,  /*{0, 0, 1} , */{145, 145, 145} },
//     //VertexInput{{-1, 1, 1}  ,  /*{1, 0, 0} , */{145, 145, 145} },
//};

//const static std::vector<uint32_t> indices =
//{
//            0,1,3,
//            3,1,2,
//            4,5,7,
//            7,5,6,
//            8,9,11,
//            11,9,10,
//            12,13,15,
//            15,13,14,
//            16,17,19,
//            19,17,18,
//            20,21,23,
//            23,21,22
//};