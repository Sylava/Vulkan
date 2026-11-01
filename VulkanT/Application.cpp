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
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetWindowUserPointer(window, this);
}

void Application::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window));
    app->framebufferResized = true;
}

void Application::InitVulkan()
{
    renderer = new Renderer();
    renderer->CreateInstance(&framebufferResized);
    renderer->createSurface(window, &surface);
    device = renderer->pickPhysicalDevice();
    renderer->createLogicalDevice();
    renderer->createSwapChain();
    renderer->createImageViews();
    renderer->createDescriptorSetLayout();
    renderer->createGraphicsPipeline();

    renderer->createCommandPool();
    renderer->createTextureImage();
    renderer->createTextureImageView();
    renderer->createTextureSampler();
    renderer->createCommandBuffer();
    renderer->createSyncObjects();
    renderer->createVertexBuffer();
    renderer->createIndexBuffer();
    renderer->createUniformBuffers();
    renderer->createDescriptorPool();
    renderer->createDescriptorSets();
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
