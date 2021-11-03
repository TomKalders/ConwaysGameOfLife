#pragma once
#include "Renderer.h"

#include <string>
#include <windows.h>
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
//#include <d3dx11effect.h>

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

	HWND GetHandle();

private:
	HWND m_Handle = NULL;
	HMENU m_Menu = NULL;
	static HINSTANCE m_Instance; /*= NULL;*/
	RECT m_Rect;

	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

	IDXGIFactory* m_pDXGIFactory;
	IDXGISwapChain* m_pSwapChain;
	ID3D11Texture2D* m_pDepthStencilBuffer;
	ID3D11DepthStencilView* m_pDepthStencilView;
	ID3D11Resource* m_pRenderTargetBuffer;
	ID3D11RenderTargetView* m_pRenderTargetView;

	
	std::string m_WindowTitle;
	std::wstring m_WindowTitleWide;
	static std::wstring m_ClassTitle;
	int m_Width;
	int m_Height;

	HRESULT InitializeDirectX();
	HRESULT CreateHandle();
	void RegisterWindowClass();

	static LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

