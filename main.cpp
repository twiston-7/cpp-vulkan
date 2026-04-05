#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

#ifndef NDEBUG
    const bool enableValidationLayers = true;
#else
    const bool enableValidationLayers = false;
#endif

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

class Render {
VkInstance instance;
GLFWwindow *window;

public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }
private:
    const uint32_t WIDTH = 800;
    const uint32_t HEIGHT = 600;
    const uint32_t VERSION = VK_MAKE_VERSION(1, 0, 0);

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Needs special care to resize, so just dont for now.
        window = glfwCreateWindow(800, 600, "GUI Test", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void setAppInfo(VkApplicationInfo appInfo) {
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "GUI Test";
        appInfo.applicationVersion = VERSION;
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VERSION;
        appInfo.apiVersion = VK_API_VERSION_1_0;
    }

    void setCreateInfo(VkInstanceCreateInfo createInfo, VkApplicationInfo *appInfo) {
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = appInfo;
    }

    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount;
        const char** glfwExtensions;
        
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionsCount);

        if (enableValidationLayers) {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return extensions;
    }

    void handleExtensions(VkInstanceCreateInfo createInfo, uint32_t glfwExtensionCount, const char** glfwExtensions) {
        glfwExtensions = getRequiredExtensions();

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;
    }

    void handleRequiredExtensions(VkInstanceCreateInfo createInfo, uint32_t glfwExtensionCount, std::vector<const char*> requiredExtensions) {
        for (uint32_t i = 0; i < glfwExtensionCount; i++) {
            requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);
        }

        createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;

        createInfo.enabledExtensionCount = (uint32_t) requiredExtensions.size();
        createInfo.ppEnabledExtensionNames = requiredExtensions.data();
    }

    void showAvailableExtensions() {
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> extensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

        std::cout << "available extensions:\n";

        for (const auto& extension : extensions) {
            std::cout << '\t' << extension.extensionName << '\n';
        }
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const VkLayerProperties &layerProperties : availableLayers) {
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

    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        VkInstanceCreateInfo createInfo{};

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;

        setAppInfo(appInfo);

        setCreateInfo(createInfo, &appInfo);

        handleExtensions(createInfo, glfwExtensionCount, glfwExtensions);

        std::vector<const char*> requiredExtensions;
        handleRequiredExtensions(createInfo, glfwExtensionCount, requiredExtensions);

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
           throw std::runtime_error("failed to create instance!");
        }

        showAvailableExtensions();
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }
};

int main() {
    Render app;
    app.run();

    return 0;
}