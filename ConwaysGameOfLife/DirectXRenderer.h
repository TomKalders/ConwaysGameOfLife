#pragma once
#include "Renderer.h"
#include "PerspectiveCamera.h"
#include "Mesh.h"

//General Includes
#include <string>
#include <windows.h>
#include "functional"

//SDL2
#include <SDL.h>

//DirectX
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

//IMGUI
#include "imgui.h"
#include "backends/imgui_impl_sdl.h"
#include "backends/imgui_impl_dx11.h"

class DirectXRenderer : public Renderer
{
public:
	DirectXRenderer(HINSTANCE hInstance, const std::string& windowName, int width, int height);
	DirectXRenderer(const DirectXRenderer & other) = delete;
	DirectXRenderer(DirectXRenderer && other) = delete;
	DirectXRenderer& operator=(const DirectXRenderer & other) = delete;
	DirectXRenderer& operator=(DirectXRenderer && other) = delete;
	virtual ~DirectXRenderer() = default;

	virtual bool Initialize(Grid* grid) override;
	virtual void Render() override;
	virtual void Cleanup() override;
	virtual int GetWindowWidth() const override;
	virtual int GetWindowHeight() const override;
	virtual void ToggleGrid() override;

	HWND GetHandle() const;
	PerspectiveCamera* GetCamera() const;
	ID3D11Device* const GetDevice() const;
	ID3D11DeviceContext* const GetDeviceContext() const;
	const std::vector<Mesh*>& GetMeshes() const;

	void AddMesh(Mesh* pMesh);
	void RemoveMesh(Mesh* pmesh);

	//Imgui
	bool UseVersionOne();

private:
	//----- Handle -----
	HRESULT CreateHandle();
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HWND m_Handle = NULL;
	HMENU m_Menu = NULL;
	static HINSTANCE m_Instance;
	RECT m_Rect = RECT{};
	//------------------

	//------ SDL2 ------
	bool InitializeSDLWindow();
	SDL_Window* m_pWindow;
	//------------------

	//----- DirectX -----
	HRESULT InitializeDirectX();
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

	IDXGIFactory* m_pDXGIFactory = nullptr;
	IDXGISwapChain* m_pSwapChain = nullptr;
	ID3D11Texture2D* m_pDepthStencilBuffer = nullptr;
	ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
	ID3D11Resource* m_pRenderTargetBuffer = nullptr;
	ID3D11RenderTargetView* m_pRenderTargetView = nullptr;

	bool m_showWireframe;
	//-------------------

	//------ IMGUI ------
	bool InitializeImGui();
	void RenderImGui();

	static const int m_Size = 128;
	char* m_Buffer = new char[m_Size]{};
	int m_Filetype = 0;
	int m_NrOfThreads = 1;
	bool m_UseVersionOne;
	glm::fvec3 m_CameraPosition;
	//-------------------

	int m_Index;
	//Mesh* initialMesh;
	////std::vector<VertexInput>& m_InitialVertexBuffer;
	//VertexInput initialVertex;
	
	PerspectiveCamera* m_pCamera = nullptr;
	std::string m_WindowTitle;
	std::wstring m_WindowTitleWide;
	static std::wstring m_ClassTitle;
	int m_Width;
	int m_Height;

	std::vector<Mesh*> m_pMeshes;

	void RenderMeshes() const;
};

