#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

class ShaderProgram;
struct GLFWwindow;

class Application
{
public:
	static Application& GetInstance()
	{
		static Application app;
		return app;
	}
	void Init();
	void Run();
	void Exit();

	void UpdateInput();
	void PostInputUpdate();
	
	static void MouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrollCallbacks(GLFWwindow* window, double xoffset, double yoffset);

	int GetWindowHeight();
	int GetWindowWidth();

	void SetWindowHeight(const int &newWindowHeight);
	void SetWindowWidth(const int &newWindowWidth);
	void SetWindowSize(const int &newWindowWidth, const int &newWindowHeight);

private:
	Application();
	~Application();

	static bool IsKeyPressed(unsigned short key);
	void InitDisplay(void);

	// Should make these not hard-coded :P
	/*const static int m_window_width = 800;
	const static int m_window_height = 600;*/
	
	int m_window_width;
	int m_window_height;

	//Declare a window object
	StopWatch m_timer;

	//Pointer to shader program
	ShaderProgram *currProg;
};

#endif