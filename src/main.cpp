#include "setup.h"
#include "renderer.h"
#include <GLFW/glfw3.h>

int main() {
    Render app;
    app.initWindow();
    app.initVulkan();

    Renderer renderer;
    renderer.init(app);

    while (!glfwWindowShouldClose(app.getWindow())) {
        glfwPollEvents();

        if (app.isFramebufferResized()) {
            app.clearFramebufferResized();
            renderer.recreateSwapChain();
        }

        renderer.drawFrame();
    }

    renderer.cleanup();
    app.cleanup();

    return 0;
}