#ifndef SETUP_H
#define SETUP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <optional>

#include <vector>
#include <string>
#include <iostream>

class Render {
    VkInstance instance{};
    GLFWwindow *window{};
    VkDevice device{};
    VkQueue graphicsQueue{};
    VkQueue presentQueue{};
    VkDebugUtilsMessengerEXT debugMessenger{};
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface{};

public:
    void run();
private:
    const int WIDTH = 800;
    const int HEIGHT = 600;
    const uint32_t VERSION = VK_MAKE_VERSION(1, 0, 0);

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        bool isComplete() {
            return graphicsFamily.has_value() && presentFamily.has_value();
        }
    };
    
    void initWindow();
    void setAppInfo(VkApplicationInfo &appInfo) const;
    static void setCreateInfo(VkInstanceCreateInfo &createInfo, const VkApplicationInfo &appInfo);
    
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    static std::vector<const char*> getRequiredExtensions();
    static void handleExtensions(const std::vector<const char*> &requiredExtensions, VkInstanceCreateInfo &createInfo);
    
    static bool checkValidationLayerSupport();
    static void handleValidationLayers(VkInstanceCreateInfo &createInfo, VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo);
    
    void createInstance();

    VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) const;
    void setupDebugMessenger();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool fitsMinimumRequirements(VkPhysicalDevice device);
    uint32_t rateDeviceSuitability(VkPhysicalDevice device);
    void pickPhysicalDevice();
    
    void createLogicalDevice();
    void createSurface();
    void createSwapChain();

    VkSwapchainKHR swapChain{};
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat{};
    VkExtent2D swapChainExtent{};
    bool framebufferResized = false;
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

    void initVulkan();

    void mainLoop();

    void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks* pAllocator
    ) {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void cleanup();
    void recreateSwapChain();
};

#endif