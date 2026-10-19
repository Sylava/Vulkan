#pragma once
#include <fstream>
#include "Dependencies.h"

class Renderer
{
public:
	void CreateInstance();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	void pickPhysicalDevice();
	void createLogicalDevice();
	void createSurface(GLFWwindow* win, vk::raii::SurfaceKHR* surface);
	vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	vk::PresentModeKHR chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes);
	vk::Extent2D chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities);
	void createSwapChain();
	uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities);
	void createImageViews();
	void createGraphicsPipeline();
	static std::vector<char> readFile(const std::string& filename);
	[[nodiscard]] vk::raii::ShaderModule createShaderModule(const std::vector<char>& code) const;

	vk::raii::Instance instance = nullptr;
	GLFWwindow* window = nullptr;
	vk::raii::SurfaceKHR* surface;

private:
	std::vector<char const*> GetRequiredExtensions();
	std::vector<char const*> GetRequiredLayers();
	void SetupDebugMessenger();

	vk::raii::Context context;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;
	std::vector<const char*> requiredDeviceExtension;
	vk::raii::Queue queue = nullptr;
	vk::Extent2D swapChainExtent;
	vk::SurfaceFormatKHR swapChainSurfaceFormat;
	vk::raii::SwapchainKHR swapChain = nullptr;
	std::vector<vk::Image> swapChainImages;
	std::vector<vk::raii::ImageView> swapChainImageViews;

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT,
		vk::DebugUtilsMessageTypeFlagsEXT,
		const vk::DebugUtilsMessengerCallbackDataEXT*,
		void*);
};

