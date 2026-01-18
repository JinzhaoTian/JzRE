/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"
#include "JzRE/Runtime/Function/ECS/JzEnttModelSpawner.h"
#include "JzRE/Runtime/Function/ECS/JzEnttRenderComponents.h"
#include "JzRE/Runtime/Resource/JzModel.h"

#include <iostream>
#include <string>

/**
 * @brief Command line arguments structure
 */
struct CommandLineArgs {
    std::string      inputModel;
    JzRE::JzERHIType graphicApi = JzRE::JzERHIType::OpenGL;
};

/**
 * @brief Print usage information
 *
 * @param programName Name of the program
 */
void PrintUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " --input <model_file> [--graphic_api <api>]\n"
              << "\n"
              << "Options:\n"
              << "  --input, -i        Path to the model file to open (required)\n"
              << "  --graphic_api, -g  Graphics API to use: opengl, vulkan (default: opengl)\n"
              << "  --help, -h         Show this help message\n"
              << "\n"
              << "Examples:\n"
              << "  " << programName << " --input model.obj\n"
              << "  " << programName << " -i model.fbx --graphic_api vulkan\n";
}

/**
 * @brief Parse command line arguments
 *
 * @param argc Argument count
 * @param argv Argument values
 * @param args Output arguments structure
 * @return true if parsing succeeded, false otherwise
 */
bool ParseCommandLine(int argc, char *argv[], CommandLineArgs &args)
{
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h") {
            PrintUsage(argv[0]);
            return false;
        } else if (arg == "--input" || arg == "-i") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --input requires a file path argument\n";
                return false;
            }
            args.inputModel = argv[++i];
        } else if (arg == "--graphic_api" || arg == "-g") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --graphic_api requires an API name argument\n";
                return false;
            }
            std::string api = argv[++i];
            if (api == "opengl" || api == "OpenGL") {
                args.graphicApi = JzRE::JzERHIType::OpenGL;
            } else if (api == "vulkan" || api == "Vulkan") {
                args.graphicApi = JzRE::JzERHIType::Vulkan;
            } else {
                std::cerr << "Error: Unknown graphics API '" << api << "'. Supported: opengl, vulkan\n";
                return false;
            }
        } else {
            std::cerr << "Error: Unknown argument '" << arg << "'\n";
            PrintUsage(argv[0]);
            return false;
        }
    }

    if (args.inputModel.empty()) {
        std::cerr << "Error: --input is required\n";
        PrintUsage(argv[0]);
        return false;
    }

    return true;
}

/**
 * @brief 3D Model Viewer application
 *
 * A command-line driven 3D rendering application that loads and displays
 * a model file specified via command line arguments using the ECS-based
 * rendering pipeline.
 */
class ModelViewer : public JzRE::JzRERuntime {
public:
    /**
     * @brief Constructor
     *
     * @param args Parsed command line arguments
     */
    explicit ModelViewer(const CommandLineArgs &args) :
        JzRERuntime(args.graphicApi, "JzRE Model Viewer", {1280, 720}),
        m_modelPath(args.inputModel)
    {
    }

protected:
    /**
     * @brief Called before the main loop starts
     */
    void OnStart() override
    {
        std::cout << "Loading model: " << m_modelPath << "\n";

        auto model = std::make_shared<JzRE::JzModel>(m_modelPath);
        if (model->Load()) {
            // Spawn model as ECS entities
            m_modelEntities = JzRE::JzEnttModelSpawner::SpawnModel(GetWorld(), model);
            std::cout << "Model loaded successfully (" << m_modelEntities.size() << " entities)\n";

            // Configure orbit camera for viewing the model
            // Get the main camera entity and modify its orbit controller
            auto mainCamera = GetMainCameraEntity();
            if (JzRE::IsValidEntity(mainCamera)) {
                auto *orbit = GetWorld().TryGetComponent<JzRE::JzEnttOrbitControllerComponent>(mainCamera);
                if (orbit) {
                    // Cornell Box approximate center: X=[-3, 2.5], Y=[-0.16, 5.3], Z=[-5.8, -0.24]
                    // Center ≈ (-0.25, 2.5, -3.0)
                    orbit->target   = JzRE::JzVec3(-0.25f, 2.5f, -3.0f);
                    orbit->distance = 8.0f;
                    orbit->pitch    = 0.2f;
                    orbit->yaw      = 0.0f;
                }
            }
        } else {
            std::cerr << "Error: Failed to load model: " << m_modelPath << "\n";
        }
    }

    /**
     * @brief Called once per frame before rendering
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     */
    void OnUpdate([[maybe_unused]] JzRE::F32 deltaTime) override
    {
        // Camera controls are now handled by the JzEnttCameraSystem
        // Additional user logic can be added here
    }

    /**
     * @brief Called after the main loop ends
     */
    void OnStop() override
    {
        std::cout << "Closing Model Viewer\n";

        // Clean up spawned entities
        JzRE::JzEnttModelSpawner::DestroyEntities(GetWorld(), m_modelEntities);
    }

private:
    std::string                       m_modelPath;
    std::vector<JzRE::JzEnttEntity>   m_modelEntities;
};

/**
 * @brief Main entry point
 *
 * @param argc Argument count
 * @param argv Argument values
 * @return int Exit code
 */
int main(int argc, char *argv[])
{
    CommandLineArgs args;

    if (!ParseCommandLine(argc, argv, args)) {
        return 1;
    }

    std::cout << "Starting JzRE Model Viewer\n"
              << "  Model: " << args.inputModel << "\n"
              << "  Graphics API: " << (args.graphicApi == JzRE::JzERHIType::OpenGL ? "OpenGL" : "Vulkan") << "\n";

    ModelViewer app(args);
    app.Run();

    return 0;
}
