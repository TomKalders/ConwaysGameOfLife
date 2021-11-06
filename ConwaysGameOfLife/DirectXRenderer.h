#pragma once
#include "Renderer.h"
#include "PerspectiveCamera.h"

#include <string>
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

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
	virtual void Render() const override;
	virtual void Cleanup() override;
	virtual int GetWindowWidth() const override;
	virtual int GetWindowHeight() const override;
	virtual void ToggleGrid() override;

	HWND GetHandle() const;
	PerspectiveCamera* GetCamera() const;

private:
	//----- Handle -----
	HRESULT CreateHandle();
	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	HWND m_Handle = NULL;
	HMENU m_Menu = NULL;
	static HINSTANCE m_Instance; /*= NULL;*/
	RECT m_Rect = RECT{};
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
	//-------------------
	
	PerspectiveCamera* m_pCamera = nullptr;
	std::string m_WindowTitle;
	std::wstring m_WindowTitleWide;
	static std::wstring m_ClassTitle;
	int m_Width;
	int m_Height;

	void Test() const;
};

