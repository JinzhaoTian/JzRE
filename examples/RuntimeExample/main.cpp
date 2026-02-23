/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"

#include <iostream>
#include <string>

/**
 * @brief Command line arguments structure
 */
struct CommandLineArgs {
    std::string      inputModel;
    std::string      projectFile;
    JzRE::JzERHIType graphicApi = JzRE::JzERHIType::Unknown;
};

/**
 * @brief Print usage information
 *
 * @param programName Name of the program
 */
void PrintUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " [--project <project_file>] [--input <model_file>] [--graphic_api <api>]\n"
              << "\n"
              << "Options:\n"
              << "  --project, -p      Path to .jzreproject file (optional, auto-configures paths)\n"
              << "  --input, -i        Path to the model file to open (required if no project)\n"
              << "  --graphic_api, -g  Graphics API to use: opengl, vulkan (default: auto)\n"
              << "  --help, -h         Show this help message\n"
              << "\n"
              << "Examples:\n"
              << "  " << programName << " --project MyGame.jzreproject\n"
              << "  " << programName << " --input model.obj\n"
              << "  " << programName << " -p MyGame.jzreproject -i model.fbx\n"
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
        } else if (arg == "--project" || arg == "-p") {
            if (i + 1 >= argc) {
                std::cerr << "Error: --project requires a file path argument\n";
                return false;
            }
            args.projectFile = argv[++i];
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

    // Either project file or input model is required
    if (args.inputModel.empty() && args.projectFile.empty()) {
        std::cerr << "Error: --project or --input is required\n";
        PrintUsage(argv[0]);
        return false;
    }

    return true;
}

/**
 * @brief JzRE Example for 3D model loading using the ECS-based Asset system
 *
 * A command-line driven 3D rendering application that loads and displays
 * a model file using the ECS-based asset management system.
 *
 * This example demonstrates:
 * - Loading projects via JzProjectManager
 * - Loading models via JzAssetSystem
 * - Spawning entities from models with SpawnModel()
 * - Automatic asset lifecycle management with DetachAllAssets()
 */
class RuntimeExample : public JzRE::JzRERuntime {
public:
    /**
     * @brief Constructor
     *
     * @param args Parsed command line arguments
     */
    explicit RuntimeExample(const CommandLineArgs &args) :
        JzRERuntime(CreateSettings(args)),
        m_modelPath(args.inputModel) { }

private:
    static JzRE::JzRERuntimeSettings CreateSettings(const CommandLineArgs &args)
    {
        JzRE::JzRERuntimeSettings settings;
        settings.windowTitle = "JzRE Example";
        settings.windowSize  = {1280, 720};
        settings.rhiType     = args.graphicApi;

        if (!args.projectFile.empty()) {
            settings.projectFile = args.projectFile;
        }

        return settings;
    }

public:

protected:
    /**
     * @brief Called before the main loop starts
     */
    void OnStart() override
    {
        // Print project info if loaded
        if (HasProject()) {
            const auto *config = GetProjectConfig();
            std::cout << "Project loaded: " << config->projectName << "\n";
            std::cout << "  Content path: " << config->GetContentPath().string() << "\n";
        }

        // If no model path specified, try to load default scene from project
        if (m_modelPath.empty() && HasProject()) {
            const auto *config = GetProjectConfig();
            if (!config->defaultScene.empty()) {
                m_modelPath = config->defaultScene;
                std::cout << "Using default scene from project: " << m_modelPath << "\n";
            } else {
                std::cout << "No model specified and no default scene in project.\n";
                return;
            }
        }

        if (m_modelPath.empty()) {
            std::cout << "No model to load.\n";
            return;
        }

        std::cout << "Loading model via Asset system: " << m_modelPath << "\n";

        auto &assetSystem = GetAssetSystem();

        // Load model synchronously
        m_modelHandle = assetSystem.LoadSync<JzRE::JzModel>(m_modelPath);

        if (!assetSystem.IsLoaded(m_modelHandle)) {
            std::cerr << "Error: Failed to load model: " << m_modelPath << "\n";
            return;
        }

        // Add reference to keep the model alive
        assetSystem.AddRef(m_modelHandle);

        // Spawn ECS entities from the model
        // SpawnModel handles: transform, mesh/material components, ref tracking, ready tags
        m_modelEntities = assetSystem.SpawnModel(GetWorld(), m_modelHandle);

        std::cout << "Model loaded successfully (" << m_modelEntities.size() << " mesh entities)\n";
    }

    /**
     * @brief Called once per frame before rendering
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     */
    void OnUpdate([[maybe_unused]] JzRE::F32 deltaTime) override
    {
        // JzAssetSystem automatically handles:
        // - Monitoring asset components for load state changes
        // - Updating component cache data when assets become ready
        // - Managing JzAssetLoadingTag / JzAssetReadyTag tags
    }

    /**
     * @brief Called after the main loop ends
     */
    void OnStop() override
    {
        std::cout << "Closing JzRE Example\n";

        auto &assetSystem = GetAssetSystem();
        auto &world       = GetWorld();

        // DetachAllAssets releases all tracked asset references automatically
        for (auto entity : m_modelEntities) {
            if (world.IsValid(entity)) {
                assetSystem.DetachAllAssets(world, entity);
                world.DestroyEntity(entity);
            }
        }
        m_modelEntities.clear();

        // Release model handle
        if (m_modelHandle.IsValid()) {
            assetSystem.Release(m_modelHandle);
        }
    }

private:
    std::string                 m_modelPath;
    JzRE::JzModelHandle         m_modelHandle;
    std::vector<JzRE::JzEntity> m_modelEntities;
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

    std::cout << "Starting JzRE Example\n";
    if (!args.projectFile.empty()) {
        std::cout << "  Project: " << args.projectFile << "\n";
    }
    if (!args.inputModel.empty()) {
        std::cout << "  Model: " << args.inputModel << "\n";
    }
    std::cout << "  Graphics API: ";
    if (args.graphicApi == JzRE::JzERHIType::OpenGL) {
        std::cout << "OpenGL";
    } else if (args.graphicApi == JzRE::JzERHIType::Vulkan) {
        std::cout << "Vulkan";
    } else {
        std::cout << "Auto";
    }
    std::cout << "\n";

    RuntimeExample app(args);
    app.Run();

    return 0;
}
