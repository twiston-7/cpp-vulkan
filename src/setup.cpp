#include "setup.h"
#include "debug_callback.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>
#include <iostream>
#include <map>
#include <optional>
#include <set>

#ifndef NDEBUG
constexpr bool enableValidationLayers = true;
#else
constexpr bool enableValidationLayers = false;
#endif

const std::vector validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

void Render::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Needs special care to resize, so just dont for now.
    window = glfwCreateWindow(WIDTH, HEIGHT, "GUI Test", nullptr, nullptr);
}

void Render::setAppInfo(VkApplicationInfo &appInfo) const {
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "GUI Test";
    appInfo.applicationVersion = VERSION;
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VERSION;
    appInfo.apiVersion = VK_API_VERSION_1_0;
}

void Render::setCreateInfo(VkInstanceCreateInfo &createInfo, const VkApplicationInfo &appInfo) {
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
}

void Render::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};

    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
}

std::vector<const char*> Render::getRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;

    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

void Render::handleExtensions(const std::vector<const char*> &requiredExtensions, VkInstanceCreateInfo &createInfo) {
    createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();
}

bool Render::checkValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) {
        bool layerFound = false;

        for (const VkLayerProperties& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

void Render::handleValidationLayers(VkInstanceCreateInfo &createInfo, VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo) {    
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        createInfo.pNext = &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }
}

void Render::createInstance() {
    if (enableValidationLayers && !checkValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    setAppInfo(appInfo);

    VkInstanceCreateInfo createInfo{};
    setCreateInfo(createInfo, appInfo);

    const std::vector<const char*> requiredExtensions = getRequiredExtensions();
    handleExtensions(requiredExtensions, createInfo);

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        handleValidationLayers(createInfo, debugCreateInfo);

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = &debugCreateInfo;
    }

    if (vkCreateInstance(
        &createInfo,
        nullptr,
        &instance
        ) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}

VkResult Render::CreateDebugUtilsMessengerEXT(
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, 
    const VkAllocationCallbacks* pAllocator, 
    VkDebugUtilsMessengerEXT* pDebugMessenger
) const {
    if (
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")
            ); func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Render::setupDebugMessenger() {
    if (!enableValidationLayers) { return; }

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);

    if (enableValidationLayers) {
        if (CreateDebugUtilsMessengerEXT(
            &createInfo, 
            nullptr, 
            &debugMessenger
        ) != VK_SUCCESS) {
            throw std::runtime_error("failed to set up debug messenger!");
        }
    }
}

Render::QueueFamilyIndices Render::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.isComplete()) {
            break;
        }
    }

    return indices;
}

bool Render::fitsMinimumRequirements(VkPhysicalDevice device) {
    Render::QueueFamilyIndices indices = findQueueFamilies(device);

    return indices.isComplete();
}

uint32_t Render::rateDeviceSuitability(VkPhysicalDevice device) {
    if (!fitsMinimumRequirements(device)) {
        throw std::runtime_error("no suitable device found!");
    }

    uint32_t score = 0;

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);

    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    std::cout << "Device found: " << deviceProperties.deviceName << "\n";
    std::cout << "With type: ";
    switch (deviceProperties.deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: std::cout << "Integrated GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: std::cout << "Discrete GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: std::cout << "Virtual GPU"; break;
        case VK_PHYSICAL_DEVICE_TYPE_CPU: std::cout << "CPU"; break;
        default: std::cout << "Other"; break;
    }

    std::cout << std::endl;

    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    score += deviceProperties.limits.maxImageDimension2D;

    return score;
}

void Render::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPU with vulkan support!");
    }

    std::vector<VkPhysicalDevice> physical_devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, physical_devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto &physical_device : physical_devices) {
        uint32_t score = rateDeviceSuitability(physical_device);
        candidates.insert(std::make_pair(score, physical_device));
    }

    int highestScore = candidates.rbegin() -> first;

    auto range = candidates.equal_range(highestScore);

    if (range.first != range.second) {
        if (highestScore > 0) {
            physicalDevice = range.first->second;
        } else {
            std::cerr << "No suitable GPU found. Continuing regardless..." << std::endl;
            physicalDevice = candidates.rbegin() -> second;
        }
    }
}

void Render::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float priority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &priority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = 0;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void Render::initVulkan() {
    createInstance();
    setupDebugMessenger();
    createSurface();
    pickPhysicalDevice();
    createLogicalDevice();
}

void Render::createSurface() {
    if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        throw std::runtime_error("failed to create window surface!");
    }
}

void Render::mainLoop() const {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void Render::cleanup() {
    if (enableValidationLayers) {
        DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
    }

    vkDestroyDevice(device, nullptr);

    vkDestroySurfaceKHR(instance, surface, nullptr);

    vkDestroyInstance(instance, nullptr);

    glfwDestroyWindow(window);

    glfwTerminate();
}

void Render::run() {
    initWindow();
    initVulkan();
    mainLoop();
    cleanup();
}
