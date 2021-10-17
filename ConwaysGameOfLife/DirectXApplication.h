#pragma once
#include "Application.h"
#include "DirectXRenderer.h"
#include <string>

class DirectXApplication : public Application
{
public:
	DirectXApplication(HINSTANCE hInstance);
	DirectXApplication(const DirectXApplication & other) = delete;
	DirectXApplication(DirectXApplication && other) = delete;
	DirectXApplication& operator=(const DirectXApplication & other) = delete;
	DirectXApplication& operator=(DirectXApplication && other) = delete;
	virtual ~DirectXApplication() = default;

	//virtual void Run() override;
private:
	virtual void Initialize() override;
	virtual void HandleInput() override;
	virtual void Update(float deltaTime) override;
	virtual void Cleanup() override;

	HWND m_Handle = NULL;
	HINSTANCE m_HInstance = NULL;
	std::string m_Title;
	int m_Width;
	int m_Height;
};

