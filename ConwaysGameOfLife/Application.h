#pragma once

class Renderer;

class Application
{
public:
	Application(Renderer* pRenderer);
	Application(const Application& other) = delete;
	Application(Application&& other) = delete;
	Application& operator=(const Application& other) = delete;
	Application& operator=(Application&& other) = delete;
	virtual ~Application() = default;

	void Run();
	static void QuitApplication();

private:
	virtual void Initialize() = 0;
	virtual void HandleInput() = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void Cleanup() = 0;

protected:
	static bool m_IsRunning;
	Renderer* m_pRenderer = nullptr;
};

