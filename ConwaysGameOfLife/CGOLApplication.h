#pragma once

class Renderer;
class Grid;
struct GLFWwindow;

class CGOLApplication final
{
public:
	CGOLApplication(Renderer* renderer);
	CGOLApplication(const CGOLApplication& other) = delete;
	CGOLApplication(CGOLApplication&& other) = delete;
	CGOLApplication& operator=(const CGOLApplication& other) = delete;
	CGOLApplication& operator=(CGOLApplication&& other) = delete;

	void Run();
	static void QuitApplication();

private:
	Grid* m_pGrid;
	Renderer* m_pRenderer;
	static bool m_IsRunning;

	void Initialize();
	void Update(float deltaTime);
	void Cleanup();

};
