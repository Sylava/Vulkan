#include <print>
#include <iostream>
#define NOMINMAX
#include <cstdint> // Necessary for uint32_t
#include <limits> // Necessary for std::numeric_limits
#include <algorithm> // Necessary for std::clamp
#include "Renderer.h"
#include "Debug.h"

void Renderer::CreateInstance()
{
    // Define application info
    constexpr vk::ApplicationInfo application_info
    {
        .pApplicationName = "Vulkan Triangle",
        .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
        .pEngineName = "Gaming Campus Vulkan",
        .engineVersion = VK_MAKE_VERSION(1, 0, 0),
        .apiVersion = vk::ApiVersion14
    };

    // Get required extensions
    std::vector requiredExtensions = GetRequiredExtensions();
    auto extensionProperties = context.enumerateInstanceExtensionProperties();

    // Check if required extensions are supported
    for (uint32_t i = 0; i < requiredExtensions.size(); ++i)
    {
        auto compareExtensions = [extension = requiredExtensions[i]](auto const& extensionProperty)
            {
                return strcmp(extensionProperty.extensionName, extension) == 0;
            };

        if (std::ranges::none_of(extensionProperties, compareExtensions))
        {
            throw std::runtime_error("Required GLFW extension not supported: " + std::string(requiredExtensions[i]));
        }
    }

    // Get required layers
    std::vector<char const*> requiredLayers = GetRequiredLayers();
    auto layerProperties = context.enumerateInstanceLayerProperties();

    // Check if required layers are supported
    auto unsupportedLayerIt = std::ranges::find_if(requiredLayers, [&layerProperties](auto const& requiredLayer)
        {
            return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
                {
                    return strcmp(layerProperty.layerName, requiredLayer) == 0;
                });
        });

    if (unsupportedLayerIt != requiredLayers.end())
    {
        throw std::runtime_error("Required Layer not supported: " + std::string(*unsupportedLayerIt));
    }

    // Create instance
    vk::InstanceCreateInfo create_info
    {
        .pApplicationInfo = &application_info,
        .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
        .ppEnabledExtensionNames = requiredExtensions.data(),
    };

    instance = vk::raii::Instance(context, create_info);
    SetupDebugMessenger();
}

void Renderer::pickPhysicalDevice()
{
    std::vector<vk::raii::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    auto const devIter = std::ranges::find_if(physicalDevices, [&](auto const& physicalDevice) { return isDeviceSuitable(physicalDevice); });
    if (devIter == physicalDevices.end())
    {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
    physicalDevice = *devIter;
}

bool Renderer::isDeviceSuitable(vk::raii::PhysicalDevice const& physicalDevice)
{
    // Check if the physicalDevice supports the Vulkan 1.3 API version
    bool supportsVulkan1_3 = physicalDevice.getProperties().apiVersion >= vk::ApiVersion13;

    // Check if any of the queue families support graphics operations
    auto queueFamilies = physicalDevice.getQueueFamilyProperties();
    bool supportsGraphics = std::ranges::any_of(queueFamilies, [](auto const& qfp) { return !!(qfp.queueFlags & vk::QueueFlagBits::eGraphics); });

    // Check if all required physicalDevice extensions are available
    requiredDeviceExtension = { vk::KHRSwapchainExtensionName };
    auto availableDeviceExtensions = physicalDevice.enumerateDeviceExtensionProperties();
    bool supportsAllRequiredExtensions =
        std::ranges::all_of(requiredDeviceExtension,
            [&availableDeviceExtensions](auto const& requiredDeviceExtension)
            {
                return std::ranges::any_of(availableDeviceExtensions,
                    [requiredDeviceExtension](auto const& availableDeviceExtension)
                    { return strcmp(availableDeviceExtension.extensionName, requiredDeviceExtension) == 0; });
            });

    // Check if the physicalDevice supports the required features (shader draw parameters, dynamic rendering and extended dynamic state)
    auto features = physicalDevice.template getFeatures2<vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();
    bool supportsRequiredFeatures = features.template get<vk::PhysicalDeviceVulkan11Features>().shaderDrawParameters &&
        features.template get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering &&
        features.template get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>().extendedDynamicState;

    // Return true if the physicalDevice meets all the criteria
    return supportsVulkan1_3 && supportsGraphics && supportsAllRequiredExtensions && supportsRequiredFeatures;
}

void Renderer::createLogicalDevice() {
    // find the index of the first queue family that supports graphics
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

    // get the first index into queueFamilyProperties which supports both graphics and present
    uint32_t queueIndex = ~0;
    for (uint32_t qfpIndex = 0; qfpIndex < queueFamilyProperties.size(); qfpIndex++)
    {
        if ((queueFamilyProperties[qfpIndex].queueFlags & vk::QueueFlagBits::eGraphics) &&
            physicalDevice.getSurfaceSupportKHR(qfpIndex, *surface))
        {
            // found a queue family that supports both graphics and present
            queueIndex = qfpIndex;
            break;
        }
    }
    if (queueIndex == ~0)
    {
        throw std::runtime_error("Could not find a queue for graphics and present -> terminating");
    }

    // query for Vulkan 1.3 features
    vk::StructureChain<vk::PhysicalDeviceFeatures2,
        vk::PhysicalDeviceVulkan11Features,
        vk::PhysicalDeviceVulkan13Features,
        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>
        featureChain = {
            {},                                    // vk::PhysicalDeviceFeatures2
            {.shaderDrawParameters = true},        // vk::PhysicalDeviceVulkan11Features
            {.dynamicRendering = true},            // vk::PhysicalDeviceVulkan13Features
            {.extendedDynamicState = true}         // vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
    };

    // create a Device
    float                     queuePriority = 0.5f;
    vk::DeviceQueueCreateInfo deviceQueueCreateInfo{ .queueFamilyIndex = queueIndex, .queueCount = 1, .pQueuePriorities = &queuePriority };
    vk::DeviceCreateInfo      deviceCreateInfo{ .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
                                               .queueCreateInfoCount = 1,
                                               .pQueueCreateInfos = &deviceQueueCreateInfo,
                                               .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
                                               .ppEnabledExtensionNames = requiredDeviceExtension.data() };

    device = vk::raii::Device(physicalDevice, deviceCreateInfo);
    queue = vk::raii::Queue(device, queueIndex, 0);

    std::vector<const char*> requiredDeviceExtension = {
    vk::KHRSwapchainExtensionName };
    auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
    std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
    std::vector<vk::PresentModeKHR> availablePresentModes = physicalDevice.getSurfacePresentModesKHR(*surface);
}

void Renderer::createSurface(GLFWwindow* win, vk::raii::SurfaceKHR* surf)
{
    window = win;
    surface = surf;
    VkSurfaceKHR       _surface;
    if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != 0) {
        throw std::runtime_error("failed to create window surface!");
    }
    *surface = vk::raii::SurfaceKHR(instance, _surface);
}

vk::SurfaceFormatKHR Renderer::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
    const auto formatIt = std::ranges::find_if(
        availableFormats,
        [](const auto& format) { return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear; });
    return formatIt != availableFormats.end() ? *formatIt : availableFormats[0];
}

vk::PresentModeKHR Renderer::chooseSwapPresentMode(std::vector<vk::PresentModeKHR> const& availablePresentModes)
{
    assert(std::ranges::any_of(availablePresentModes, [](auto presentMode) { return presentMode == vk::PresentModeKHR::eFifo; }));
    return std::ranges::any_of(availablePresentModes,
        [](const vk::PresentModeKHR value) { return vk::PresentModeKHR::eMailbox == value; }) ?
        vk::PresentModeKHR::eMailbox :
        vk::PresentModeKHR::eFifo;
}

vk::Extent2D Renderer::chooseSwapExtent(vk::SurfaceCapabilitiesKHR const& capabilities)
{
    // currentExtent is only set to the special "undefined" value described above
    // when the window manager lets us choose the extent ourselves; any other value
    // means the surface already dictates a fixed extent that we must use as-is.
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    return {
        std::clamp<uint32_t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
    };
}

void Renderer::createSwapChain()
{
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
    swapChainExtent = chooseSwapExtent(surfaceCapabilities);
    uint32_t minImageCount = chooseSwapMinImageCount(surfaceCapabilities);

    std::vector<vk::SurfaceFormatKHR> availableFormats = physicalDevice.getSurfaceFormatsKHR(*surface);
    swapChainSurfaceFormat = chooseSwapSurfaceFormat(availableFormats);
    uint32_t minImageCount = surfaceCapabilities.minImageCount;
    vk::SwapchainCreateInfoKHR swapChainCreateInfo{.surface          = *surface,
                                               .minImageCount    = minImageCount,
                                               .imageFormat      = swapChainSurfaceFormat.format,
                                               .imageColorSpace  = swapChainSurfaceFormat.colorSpace,
                                               .imageExtent      = swapChainExtent,
                                               .imageArrayLayers = 1,
                                               .imageUsage       = vk::ImageUsageFlagBits::eColorAttachment,
                                               .imageSharingMode = vk::SharingMode::eExclusive,
                                               .preTransform     = surfaceCapabilities.currentTransform,
                                               .compositeAlpha   = vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                               .presentMode      = chooseSwapPresentMode(availablePresentModes),
                                               .clipped          = true};
    };
}

uint32_t Renderer::chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities)
{
    auto minImageCount = std::max(3u, surfaceCapabilities.minImageCount);
    if ((0 < surfaceCapabilities.maxImageCount) && (surfaceCapabilities.maxImageCount < minImageCount))
    {
        minImageCount = surfaceCapabilities.maxImageCount;
    }
    return minImageCount;
}

vk::raii::Instance& Renderer::GetInstance()
{
    return instance;
}

std::vector<char const*> Renderer::GetRequiredExtensions()
{
    uint32_t glfw_extension_count = 0;
    const auto glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

    std::vector extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

    if (Debug::EnableDebug)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

std::vector<char const*> Renderer::GetRequiredLayers()
{
    const std::vector validationLayers = { "VK_LAYER_KHRONOS_validation" };

    std::vector<char const*> requiredLayers = {};

    if (Debug::EnableDebug)
        requiredLayers.append_range(validationLayers);

    return requiredLayers;
}

void Renderer::SetupDebugMessenger()
{
    vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
    vk::DebugUtilsMessageTypeFlagsEXT typeFlags(vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);

    vk::DebugUtilsMessengerCreateInfoEXT createInfo{
        .messageSeverity = severityFlags,
        .messageType = typeFlags,
        .pfnUserCallback = &DebugCallback,
    };
    debugMessenger = instance.createDebugUtilsMessengerEXT(createInfo);
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL Renderer::DebugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* data, void*)
{
    std::println(std::cout, "Validation Layer : Type [{}]", vk::to_string(type));
    std::println(std::cout, "Message : {}", data->pMessage);

    return vk::False;
}
