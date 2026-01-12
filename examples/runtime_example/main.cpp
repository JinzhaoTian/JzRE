/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"
#include "JzRE/Runtime/Function/Scene/JzScene.h"
#include "JzRE/Runtime/Function/Rendering/JzRHIRenderer.h"

/**
 * @brief Example application demonstrating JzRERuntime usage
 *
 * This example shows how to create a simple rendering application
 * using the JzRERuntime class without Editor features.
 */
class RuntimeExample : public JzRE::JzRERuntime {
public:
    /**
     * @brief Constructor
     */
    RuntimeExample() :
        JzRERuntime(JzRE::JzERHIType::OpenGL, "JzRE Runtime Example", {1280, 720})
    {
    }

protected:
    /**
     * @brief Called before the main loop starts
     */
    void OnStart() override
    {
        // Get the scene and add objects
        auto scene = GetScene();

        // You can add entities, models, and other scene objects here
        // Example:
        // scene->AddModel("path/to/model.obj");
    }

    /**
     * @brief Called once per frame before rendering
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     */
    void OnUpdate([[maybe_unused]] JzRE::F32 deltaTime) override
    {
        // Update game logic here
        // Example: Update animations, physics, camera movement, etc.

        // Access the renderer if needed
        // auto& renderer = GetRenderer();

        // Access the window if needed
        // auto& window = GetWindow();
    }

    /**
     * @brief Called after the main loop ends
     */
    void OnStop() override
    {
        // Cleanup resources here
    }
};

/**
 * @brief Main entry point
 *
 * @return int Exit code
 */
int main()
{
    RuntimeExample app;
    app.Run();
    return 0;
}
