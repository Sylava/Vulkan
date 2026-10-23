#pragma once
#include <fstream>
#include "Dependencies.h"

constexpr int MAX_FRAMES_IN_FLIGHT = 2;

class Renderer
{
public:
	void CreateInstance();
	bool isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice);
	vk::raii::Device* pickPhysicalDevice();
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
	void createCommandPool();
	void createCommandBuffer();
	void recordCommandBuffer(uint32_t imageIndex);
	void transition_image_layout(
		uint32_t                imageIndex,
		vk::ImageLayout         old_layout,
		vk::ImageLayout         new_layout,
		vk::AccessFlags2        src_access_mask,
		vk::AccessFlags2        dst_access_mask,
		vk::PipelineStageFlags2 src_stage_mask,
		vk::PipelineStageFlags2 dst_stage_mask);
	void createSyncObjects();
	void drawFrame();
	void createCommandBuffers();

	
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

	// Fixed Functions
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vk::PipelineInputAssemblyStateCreateInfo inputAssembly{ .topology = vk::PrimitiveTopology::eTriangleList };
	vk::Viewport viewport{ 0.0f, 0.0f, static_cast<float>(swapChainExtent.width), static_cast<float>(swapChainExtent.height), 0.0f, 1.0f };
	vk::Rect2D scissor{ vk::Offset2D{ 0, 0 }, swapChainExtent };
	std::vector<vk::DynamicState> dynamicStates = { vk::DynamicState::eViewport, vk::DynamicState::eScissor };
	vk::PipelineDynamicStateCreateInfo dynamicState{ .dynamicStateCount = static_cast<uint32_t>(dynamicStates.size()), .pDynamicStates = dynamicStates.data() };
	vk::PipelineViewportStateCreateInfo viewportState{ .viewportCount = 1, .pViewports = &viewport, .scissorCount = 1, .pScissors = &scissor };
	vk::PipelineRasterizationStateCreateInfo rasterizer{ .depthClampEnable = vk::False,
													.rasterizerDiscardEnable = vk::False,
													.polygonMode = vk::PolygonMode::eFill,
													.cullMode = vk::CullModeFlagBits::eBack,
													.frontFace = vk::FrontFace::eClockwise,
													.depthBiasEnable = vk::False,
													.lineWidth = 1.0f };
	vk::PipelineMultisampleStateCreateInfo multisampling{ .rasterizationSamples = vk::SampleCountFlagBits::e1, .sampleShadingEnable = vk::False };
	vk::PipelineColorBlendAttachmentState colorBlendAttachment{
	.blendEnable = vk::True,
	.srcColorBlendFactor = vk::BlendFactor::eSrcAlpha,
	.dstColorBlendFactor = vk::BlendFactor::eOneMinusSrcAlpha,
	.colorBlendOp = vk::BlendOp::eAdd,
	.srcAlphaBlendFactor = vk::BlendFactor::eOne,
	.dstAlphaBlendFactor = vk::BlendFactor::eZero,
	.alphaBlendOp = vk::BlendOp::eAdd,
	.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA };
	vk::PipelineColorBlendStateCreateInfo colorBlending{
	.logicOpEnable = vk::False, .logicOp = vk::LogicOp::eCopy, .attachmentCount = 1, .pAttachments = &colorBlendAttachment };
	vk::raii::PipelineLayout pipelineLayout = nullptr;

	// Conclusion
	vk::raii::Pipeline graphicsPipeline = nullptr;

	// Command buffer
	vk::raii::CommandPool commandPool = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;
	uint32_t queueIndex = ~0;

	std::vector<vk::raii::CommandBuffer> commandBuffers;
	std::vector<vk::raii::Semaphore> presentCompleteSemaphores;
	std::vector<vk::raii::Semaphore> renderFinishedSemaphores;
	std::vector<vk::raii::Fence> inFlightFences;

	uint32_t frameIndex = 0;


	static VKAPI_ATTR vk::Bool32 VKAPI_CALL DebugCallback(
		vk::DebugUtilsMessageSeverityFlagBitsEXT,
		vk::DebugUtilsMessageTypeFlagsEXT,
		const vk::DebugUtilsMessengerCallbackDataEXT*,
		void*);
};

