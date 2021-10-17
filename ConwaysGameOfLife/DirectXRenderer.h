#pragma once
#include "Renderer.h"

#include <string>
#include <windows.h>

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

private:
	HWND m_Handle = NULL;
	HINSTANCE m_Instance = NULL;
	std::string m_WindowTitle;
	std::wstring m_WindowTitleWide;
	std::wstring m_ClassTitle;
	int m_Width;
	int m_Height;

	void RegisterWindowClass();
};

