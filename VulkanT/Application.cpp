#include "Application.h"
#include <iostream>

void Application::Init()
{
    InitWindow();
    InitVulkan();
}

void Application::InitWindow()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
}

void Application::InitVulkan()
{
    renderer = new Renderer();
    renderer->CreateInstance();
    renderer->createSurface(window, &surface);
    device = renderer->pickPhysicalDevice();
    renderer->createLogicalDevice();
    renderer->createSwapChain();
    renderer->createImageViews();
    renderer->createGraphicsPipeline();

    renderer->createCommandPool();
    renderer->createCommandBuffer();
    renderer->createSyncObjects();
}

void Application::Run()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        renderer->drawFrame();
    }
    device->waitIdle();
}

void Application::Destroy()
{
    DestroyVulkan();
    DestroyWindow();
}

void Application::DestroyWindow()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Application::DestroyVulkan()
{
}
