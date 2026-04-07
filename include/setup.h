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

public:
    void run();

private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const uint32_t VERSION = VK_MAKE_VERSION(1, 0, 0);
    void initWindow();
    void setAppInfo(VkApplicationInfo &appInfo);
    void setCreateInfo(VkInstanceCreateInfo &createInfo, VkApplicationInfo *appInfo);
    std::vector<const char*> getRequiredExtensions();
    void handleExtensions(VkInstanceCreateInfo &createInfo);
    void handleRequiredExtensions(VkInstanceCreateInfo &createInfo, uint32_t glfwExtensionCount, std::vector<const char*> requiredExtensions);
    void showAvailableExtensions();
    bool checkValidationLayerSupport();
    void createInstance();
    void mainLoop();
    void cleanup();
};

#endif