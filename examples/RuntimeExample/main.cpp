/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/JzRERuntime.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzEnttComponents.h"
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
 * @brief JzRE Example for 3D model loading using the ECS-based Asset system
 *
 * A command-line driven 3D rendering application that loads and displays
 * a model file using the ECS-based asset management system.
 *
 * This example demonstrates:
 * - Loading models via JzAssetManager
 * - Creating entities with JzMeshAssetComponent and JzMaterialAssetComponent
 * - Using JzAssetLoadingSystem for automatic asset state management
 * - Proper asset lifecycle management with reference counting
 */
class RuntimeExample : public JzRE::JzRERuntime {
public:
    /**
     * @brief Constructor
     *
     * @param args Parsed command line arguments
     */
    explicit RuntimeExample(const CommandLineArgs &args) :
        JzRERuntime(JzRE::JzRERuntimeSettings{"JzRE Example", {1280, 720}, args.graphicApi}),
        m_modelPath(args.inputModel) { }

protected:
    /**
     * @brief Called before the main loop starts
     */
    void OnStart() override
    {
        std::cout << "Loading model via Asset system: " << m_modelPath << "\n";

        auto &assetManager = GetAssetManager();

        // Load model synchronously using the new Asset system
        m_modelHandle = assetManager.LoadSync<JzRE::JzModel>(m_modelPath);

        if (!m_modelHandle.IsValid()) {
            std::cerr << "Error: Failed to load model: " << m_modelPath << "\n";
            return;
        }

        // Check load state
        auto loadState = assetManager.GetLoadState(m_modelHandle);
        if (loadState != JzRE::JzEAssetLoadState::Loaded) {
            std::cerr << "Error: Model not in loaded state\n";
            return;
        }

        // Get the loaded model
        JzRE::JzModel *model = assetManager.Get(m_modelHandle);
        if (!model) {
            std::cerr << "Error: Failed to get model data\n";
            return;
        }

        // Add reference to keep the model alive
        assetManager.AddRef(m_modelHandle);

        // Spawn ECS entities from the model using asset components
        SpawnModelEntities(model);

        std::cout << "Model loaded successfully (" << m_modelEntities.size() << " mesh entities)\n";
    }

    /**
     * @brief Called once per frame before rendering
     *
     * @param deltaTime Time elapsed since the last frame in seconds
     */
    void OnUpdate([[maybe_unused]] JzRE::F32 deltaTime) override
    {
        // The JzAssetLoadingSystem automatically handles:
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

        // Clean up spawned entities
        auto &world = GetWorld();
        for (auto entity : m_modelEntities) {
            if (world.IsValid(entity)) {
                world.DestroyEntity(entity);
            }
        }
        m_modelEntities.clear();

        // Release mesh and material handles
        auto &assetManager = GetAssetManager();
        for (auto &meshHandle : m_meshHandles) {
            if (meshHandle.IsValid()) {
                assetManager.Release(meshHandle);
            }
        }
        m_meshHandles.clear();

        for (auto &matHandle : m_materialHandles) {
            if (matHandle.IsValid()) {
                assetManager.Release(matHandle);
            }
        }
        m_materialHandles.clear();

        // Release model handle
        if (m_modelHandle.IsValid()) {
            assetManager.Release(m_modelHandle);
        }
    }

private:
    /**
     * @brief Spawn ECS entities from a loaded model using asset components
     *
     * Creates one entity per mesh with:
     * - JzTransformComponent
     * - JzMeshAssetComponent (references mesh via handle)
     * - JzMaterialAssetComponent (references material via handle)
     * - JzAssetReadyTag (since assets are already loaded)
     *
     * @param model The loaded model
     */
    void SpawnModelEntities(JzRE::JzModel *model)
    {
        auto &world        = GetWorld();
        auto &assetManager = GetAssetManager();

        const auto &meshes    = model->GetMeshes();
        const auto &materials = model->GetMaterials();

        for (JzRE::Size i = 0; i < meshes.size(); ++i) {
            const auto &mesh = meshes[i];
            if (!mesh) continue;

            // Create entity
            auto entity = world.CreateEntity();
            m_modelEntities.push_back(entity);

            // Add transform component (identity transform)
            world.AddComponent<JzRE::JzTransformComponent>(entity);

            // Create mesh handle by registering the mesh with asset manager
            // Use model path + mesh index as unique identifier
            auto  meshPath     = m_modelPath + "#mesh" + std::to_string(i);
            auto &meshRegistry = assetManager.GetRegistry<JzRE::JzMesh>();
            auto  meshHandle   = meshRegistry.Allocate(meshPath);
            if (meshHandle.IsValid()) {
                meshRegistry.Set(meshHandle, mesh);
                meshRegistry.SetLoadState(meshHandle, JzRE::JzEAssetLoadState::Loaded);
                assetManager.AddRef(meshHandle);
                m_meshHandles.push_back(meshHandle);

                // Add mesh asset component
                auto &meshComp         = world.AddComponent<JzRE::JzMeshAssetComponent>(entity, meshHandle);
                meshComp.isReady       = true;
                meshComp.indexCount    = mesh->GetIndexCount();
                meshComp.materialIndex = mesh->GetMaterialIndex();
            }

            // Get associated material (if any)
            JzRE::I32 matIdx = mesh->GetMaterialIndex();
            if (matIdx >= 0 && static_cast<JzRE::Size>(matIdx) < materials.size()) {
                const auto &material = materials[matIdx];
                if (material) {
                    // Create material handle using model path + material index
                    auto  matPath     = m_modelPath + "#mat" + std::to_string(matIdx);
                    auto &matRegistry = assetManager.GetRegistry<JzRE::JzMaterial>();
                    auto  matHandle   = matRegistry.Allocate(matPath);
                    if (matHandle.IsValid()) {
                        matRegistry.Set(matHandle, material);
                        matRegistry.SetLoadState(matHandle, JzRE::JzEAssetLoadState::Loaded);
                        assetManager.AddRef(matHandle);
                        m_materialHandles.push_back(matHandle);

                        // Add material asset component with cached properties
                        auto       &matComp   = world.AddComponent<JzRE::JzMaterialAssetComponent>(entity, matHandle);
                        const auto &props     = material->GetProperties();
                        matComp.ambientColor  = props.ambientColor;
                        matComp.diffuseColor  = props.diffuseColor;
                        matComp.specularColor = props.specularColor;
                        matComp.shininess     = props.shininess;
                        matComp.opacity       = props.opacity;
                        matComp.baseColor     = JzRE::JzVec4(props.diffuseColor.x, props.diffuseColor.y,
                                                             props.diffuseColor.z, props.opacity);
                        matComp.isReady       = true;

                        // Set diffuse texture flag if material has one
                        matComp.hasDiffuseTexture = material->HasDiffuseTexture();
                    }
                }
            }

            // Mark entity as ready (all assets already loaded)
            world.GetRegistry().emplace<JzRE::JzAssetReadyTag>(entity);
        }
    }

    std::string                         m_modelPath;
    JzRE::JzModelHandle                 m_modelHandle;
    std::vector<JzRE::JzMeshHandle>     m_meshHandles;
    std::vector<JzRE::JzMaterialHandle> m_materialHandles;
    std::vector<JzRE::JzEnttEntity>     m_modelEntities;
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

    std::cout << "Starting JzRE Example\n"
              << "  Model: " << args.inputModel << "\n"
              << "  Graphics API: " << (args.graphicApi == JzRE::JzERHIType::OpenGL ? "OpenGL" : "Vulkan") << "\n";

    RuntimeExample app(args);
    app.Run();

    return 0;
}
