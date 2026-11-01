#pragma once
#include <fstream>
#include "Dependencies.h"
#include "vertex.h"

constexpr int MAX_FRAMES_IN_FLIGHT = 2;


class Renderer
{
public:
	void CreateInstance(bool* buffer);
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
		vk::PipelineStageFlags2 dst_stage_mask,
		vk::ImageAspectFlags    image_aspect_flags);
	void createSyncObjects();
	void drawFrame();
	void createCommandBuffers();
	void recreateSwapChain();
	void cleanupSwapChain();
	void cleanup();
	void createVertexBuffer();
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
	std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> createBuffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties);
	void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);
	void createIndexBuffer();
	void createDescriptorSetLayout();
	void createUniformBuffers();
	void updateUniformBuffer(uint32_t currentImage);
	void createDescriptorPool();
	void createDescriptorSets();
	void createTextureImage();
	std::pair<vk::raii::Image, vk::raii::DeviceMemory> createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties);
	void endSingleTimeCommands(vk::raii::CommandBuffer&& commandBuffer);
	vk::raii::CommandBuffer beginSingleTimeCommands();
	void transitionImageLayout(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Image& image, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void createTextureImageView();
	vk::raii::ImageView createImageView(vk::Image const& image, vk::Format format, vk::ImageAspectFlags aspectFlags);
	void createTextureSampler();
	void copyBufferToImage(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);
	void createDepthResources();
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features);
	vk::Format findDepthFormat();

	
	vk::raii::Instance instance = nullptr;
	GLFWwindow* window = nullptr;
	vk::raii::SurfaceKHR* surface;
	bool* framebufferResized;
	vk::raii::Buffer vertexBuffer = nullptr;
	vk::raii::DeviceMemory vertexBufferMemory = nullptr;
	vk::raii::Buffer indexBuffer = nullptr;
	vk::raii::DeviceMemory indexBufferMemory = nullptr;
	std::vector<vk::raii::Buffer> uniformBuffers;
	std::vector<vk::raii::DeviceMemory> uniformBuffersMemory;
	std::vector<void*> uniformBuffersMapped;
	vk::raii::DescriptorPool descriptorPool = nullptr;
	std::vector<vk::raii::DescriptorSet> descriptorSets;
	vk::raii::Image textureImage = nullptr;
	vk::raii::DeviceMemory textureImageMemory = nullptr;
	vk::raii::ImageView textureImageView = nullptr;
	vk::raii::Sampler textureSampler = nullptr;
	vk::raii::Image depthImage = nullptr;
	vk::raii::DeviceMemory depthImageMemory = nullptr;
	vk::raii::ImageView depthImageView = nullptr;
	vk::PipelineDepthStencilStateCreateInfo depthStencil{
	.depthTestEnable = vk::True,
	.depthWriteEnable = vk::True,
	.depthCompareOp = vk::CompareOp::eLess,
	.depthBoundsTestEnable = vk::False,
	.stencilTestEnable = vk::False };

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
													.frontFace = vk::FrontFace::eCounterClockwise,
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
	vk::raii::DescriptorSetLayout descriptorSetLayout = nullptr;
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

