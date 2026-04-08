#ifndef SETUP_H
#define SETUP_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include <vulkan/vulkan.h>

class Render {
    VkInstance instance;
    GLFWwindow *window;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

public:
    void run();

private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const uint32_t VERSION = VK_MAKE_VERSION(1, 0, 0);

    void initWindow();
    void setAppInfo(VkApplicationInfo &appInfo);
    void setCreateInfo(VkInstanceCreateInfo &createInfo, VkApplicationInfo &appInfo);
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    std::vector<const char*> getRequiredExtensions();
    void handleExtensions(VkInstanceCreateInfo &createInfo, uint32_t glfwExtensionCount, const char** glfwExtensions, std::vector<const char*> &requiredExtensions);
    void showAvailableExtensions();
    bool checkValidationLayerSupport();
    void createInstance();
    void pickPhysicalDevice();
    void mainLoop();
    void handleValidationLayers(VkInstanceCreateInfo &createInfo, VkDebugUtilsMessengerCreateInfoEXT &debugCreateInfo);

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
};

#endif