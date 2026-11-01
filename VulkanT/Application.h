#pragma once
#include "Dependencies.h"
#include "Renderer.h"

class Application
{
public:
	int width = 800;
	int height = 600;

	GLFWwindow* window = nullptr;
	Renderer* renderer;
	vk::raii::SurfaceKHR surface = nullptr;
	vk::raii::Device* device;
	bool framebufferResized = false;

	void Init();
	void Run();
	void Destroy();
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
	void InitWindow();
	void InitVulkan();
	void DestroyWindow();
	void DestroyVulkan();
	void createSurface();
	void mainLoop();
};

