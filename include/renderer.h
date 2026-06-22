#ifndef RENDERER_H
#define RENDERER_H

#include "setup.h"
#include <vulkan/vulkan.h>
#include <vector>

class Renderer {
public:
    Renderer();
    ~Renderer();

    void init(Render &setup);
    void cleanup();
    void recreateSwapChain();
    void drawFrame();

private:
    Render *setupRef{nullptr};

    VkDevice device{VK_NULL_HANDLE};
    VkPhysicalDevice physicalDevice{VK_NULL_HANDLE};
    VkQueue graphicsQueue{VK_NULL_HANDLE};
    VkQueue presentQueue{VK_NULL_HANDLE};

    VkRenderPass renderPass{VK_NULL_HANDLE};
    std::vector<VkFramebuffer> swapChainFramebuffers;
    VkPipelineLayout pipelineLayout{VK_NULL_HANDLE};
    VkPipeline graphicsPipeline{VK_NULL_HANDLE};
    VkShaderModule vertShaderModule{VK_NULL_HANDLE};
    VkShaderModule fragShaderModule{VK_NULL_HANDLE};

    VkCommandPool commandPool{VK_NULL_HANDLE};
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;

    size_t currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;

    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createCommandBuffers();
    void createSyncObjects();
    void createRenderFinishedSemaphores();
    void cleanupSwapChainResources();
};

#endif

