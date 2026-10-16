#pragma once
#include "Dependencies.h"

class Renderer
{
public:
	void CreateInstance();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	void pickPhysicalDevice();
	void createLogicalDevice();


private:
	vk::raii::Context context;
	vk::raii::Instance instance = nullptr;
	vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;
	vk::raii::PhysicalDevice physicalDevice = nullptr;
	vk::raii::Device device = nullptr;
	vk::raii::Queue graphicsQueue = nullptr;

	std::vector<char const*> GetRequiredExtensions();
	std::vector<char const*> GetRequiredLayers();

	void SetupDebugMessenger();

	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT,
		vk::DebugUtilsMessageTypeFlagsEXT,
		const vk::DebugUtilsMessengerCallbackDataEXT*,
		void*);
};

