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
	void Initialize(Grid* grid) override;
	void Render() const override;
	void Cleanup() override;
	int GetWindowWidth() const override;
	int GetWindowHeight() const override;
	void ToggleGrid() override;

private:
	HINSTANCE m_HInstance = NULL;
};

