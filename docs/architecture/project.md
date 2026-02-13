# JzRE Project Management

## Overview

The Project module provides runtime-level project lifecycle management, similar to Unreal Engine's `.uproject` files. It resides in the Function Layer because project configuration drives runtime behavior rather than being a resource asset.

---

## Architecture

### Design Philosophy

1. **Function Layer owns project semantics** - Project files define runtime entry points, startup scenes, module lists, and runtime configuration - all runtime framework concerns.

2. **Resource Layer provides I/O only** - JSON parsing and file operations can use Resource Layer utilities, but project business logic stays in Function Layer.

3. **Runtime independence** - Projects can be loaded and run without Editor or UI dependencies.

---

## Components

### JzProjectConfig

Core data structure representing project configuration. Serialized to/from `.jzreproject` files.

```cpp
struct JzProjectConfig {
    // Runtime Essential
    String projectName;
    String projectId;           // UUID for caching/migration
    String engineVersion;
    std::filesystem::path rootPath;
    std::filesystem::path contentRoot;
    std::filesystem::path configRoot;
    String defaultScene;
    JzEStartupMode startupMode;
    JzERenderAPI renderAPI;
    std::vector<JzETargetPlatform> targetPlatforms;

    // Resources & Build
    std::filesystem::path assetRegistry;
    std::filesystem::path shaderCache;
    std::filesystem::path buildOutput;
    std::vector<JzImportRule> importRules;

    // Modules & Plugins
    std::vector<String> modules;
    std::vector<JzPluginEntry> plugins;
    std::vector<std::filesystem::path> pluginSearchPaths;

    // Version & Migration
    U32 projectVersion;
    String minCompatibleVersion;
};
```

### JzProjectManager

Manages project lifecycle operations:

- **LoadProject()** - Parse and load `.jzreproject` file
- **CreateProject()** - Initialize new project with default structure
- **SaveProject()** - Persist configuration changes
- **CloseProject()** - Cleanup and notify listeners
- **Event callbacks** - OnProjectLoaded, OnProjectClosing, OnProjectSaved

---

## File Format

### .jzreproject (JSON)

```json
{
  "project_name": "MyGame",
  "project_id": "a1b2c3d4e5f6",
  "engine_version": "1.0.0",
  "content_root": "Content",
  "config_root": "Config",
  "default_scene": "Scenes/MainMenu.scene",
  "startup_mode": "Authoring",
  "render_api": "Auto",
  "target_platforms": ["Windows", "Linux"],
  "asset_registry": "AssetRegistry.json",
  "shader_cache": "Intermediate/ShaderCache",
  "build_output": "Build",
  "modules": [],
  "plugins": [],
  "project_version": 1,
  "min_compatible_version": "1.0.0"
}
```

### .workspace (JSON, Optional)

Workspace-level host settings stored separately:

```json
{
  "workspace_layout": "Config/WorkspaceLayout.json",
  "recent_scenes": ["Scenes/Level1.scene", "Scenes/MainMenu.scene"],
  "workspace_settings_file": "Config/Workspace.json"
}
```

`JzProjectManager` reads legacy `.editor` files and legacy keys (`editor_layout`,
`editor_settings_file`) for backward compatibility, but writes the new `.workspace`
format and keys.
When legacy settings are loaded successfully, the manager also writes a migrated
`.workspace` file automatically.

---

## Directory Structure

When a new project is created:

```
MyGame/
├── MyGame.jzreproject      # Project configuration
├── MyGame.workspace        # Workspace settings (optional)
├── Content/                # Asset root
├── Config/                 # Configuration files
├── Intermediate/           # Cached/generated files
│   └── ShaderCache/
└── Build/                  # Build outputs
```

---

## Usage

### Loading a Project

```cpp
JzProjectManager manager;

auto result = manager.LoadProject("path/to/MyGame.jzreproject");
if (result == JzEProjectResult::Success) {
    const auto& config = manager.GetConfig();

    // Access project paths
    auto contentPath = config.GetContentPath();
    auto defaultScene = config.defaultScene;

    // Initialize systems based on config
    InitializeRenderer(config.renderAPI);
    LoadScene(contentPath / defaultScene);
}
```

### Creating a New Project

```cpp
JzProjectManager manager;

auto result = manager.CreateProject("path/to/NewProject", "My New Game");
if (result == JzEProjectResult::Success) {
    // Project directory structure created
    // Default .jzreproject file written
}
```

### Listening to Project Events

```cpp
manager.OnProjectLoaded([](const JzProjectConfig& config) {
    LOG_INFO("Project loaded: {}", config.projectName);
});

manager.OnProjectClosing([](const JzProjectConfig& config) {
    // Save any pending changes
});
```

---

## Integration Points

### With JzRERuntime

```cpp
class JzRERuntime {
    JzProjectManager m_projectManager;

    void Initialize(const std::filesystem::path& projectFile) {
        if (m_projectManager.LoadProject(projectFile) == JzEProjectResult::Success) {
            const auto& config = m_projectManager.GetConfig();
            // Configure runtime based on project settings
        }
    }
};
```

### With JzAssetManager

```cpp
// Use project paths for asset loading
auto& config = projectManager.GetConfig();
assetManager.SetContentRoot(config.GetContentPath());
assetManager.LoadRegistry(config.GetAssetRegistryPath());
```

### With Host Tooling

```cpp
// Host tooling can load/save workspace-specific settings
auto workspaceSettings = projectManager.LoadWorkspaceSettings();
if (workspaceSettings) {
    LoadWorkspaceLayout(workspaceSettings->workspaceLayout);
}
```

---

## Include Path

```cpp
#include "JzRE/Runtime/Function/Project/JzProjectConfig.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"
```
