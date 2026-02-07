/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Core/JzEditorState.h"
#include "JzRE/Editor/Panels/JzHierarchy.h"
#include "JzRE/Editor/UI/JzTreeNode.h"
#include "JzRE/Editor/UI/JzGroup.h"
#include "JzRE/Editor/UI/JzSeparator.h"
#include "JzRE/Editor/UI/JzButton.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Runtime/Function/ECS/JzWorld.h"
#include "JzRE/Runtime/Function/ECS/JzEntityComponents.h"
#include "JzRE/Runtime/Function/ECS/JzTransformComponents.h"
#include "JzRE/Runtime/Function/ECS/JzAssetComponents.h"
#include "JzRE/Runtime/Function/ECS/JzAssetSystem.h"
#include "JzRE/Runtime/Function/Project/JzProjectManager.h"
#include "JzRE/Runtime/Function/Asset/JzAssetImporter.h"
#include "JzRE/Runtime/Platform/Dialog/JzOpenFileDialog.h"

JzRE::JzHierarchy::JzHierarchy(const JzRE::String &name, JzRE::Bool is_opened) :
    JzPanelWindow(name, is_opened),
    m_actions(CreateWidget<JzGroup>()),
    m_actors(CreateWidget<JzGroup>())
{
    // Create action buttons
    auto &addModelButton         = m_actions.CreateWidget<JzButton>("+ Add Model");
    addModelButton.ClickedEvent += [this]() {
        AddModelFromFile();
    };

    auto &createButton         = m_actions.CreateWidget<JzButton>("+ Create Entity");
    createButton.ClickedEvent += [this]() {
        CreateEmptyEntity();
    };

    auto &deleteButton         = m_actions.CreateWidget<JzButton>("- Delete");
    deleteButton.ClickedEvent += [this]() {
        DeleteSelectedEntity();
    };

    CreateWidget<JzSeparator>();
}

void JzRE::JzHierarchy::Update(JzRE::F32 deltaTime)
{
    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    if (m_needsRefresh) {
        RefreshEntityList();
        m_needsRefresh = false;
    }
}

void JzRE::JzHierarchy::RefreshEntityList()
{
    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    Clear();

    auto &world = JzServiceContainer::Get<JzWorld>();

    // Query all entities with JzNameComponent
    auto view = world.View<JzNameComponent>();
    for (auto [entity, nameComp] : view.each()) {
        // Create tree node for each entity
        auto &node = m_actors.CreateWidget<JzTreeNode>(nameComp.name, false);
        node.leaf  = true;

        // Store entity-widget mapping
        m_entityWidgetMap[entity] = &node;

        // Wire click event to selection
        node.ClickedEvent += [this, entity, &node]() {
            SelectActorByWidget(node);
            EntitySelectedEvent.Invoke(entity);
        };
    }
}

void JzRE::JzHierarchy::Clear()
{
    m_actors.RemoveAllWidgets();
    m_entityWidgetMap.clear();
}

void JzRE::JzHierarchy::UnselectActorsWidgets()
{
    for (auto &[entity, widget] : m_entityWidgetMap) {
        widget->selected = false;
    }
}

void ExpandTreeNode(JzRE::JzTreeNode &node)
{
    node.Open();

    if (node.HasParent()) {
        if (auto parent = dynamic_cast<JzRE::JzTreeNode *>(node.GetParent()); parent) {
            ExpandTreeNode(*parent);
        }
    }
}

void JzRE::JzHierarchy::SelectActorByWidget(JzRE::JzTreeNode &widget)
{
    UnselectActorsWidgets();

    widget.selected = true;

    if (widget.HasParent()) {
        if (auto parent = dynamic_cast<JzTreeNode *>(widget.GetParent()); parent) {
            ExpandTreeNode(*parent);
        }
    }
}

void JzRE::JzHierarchy::SelectEntity(JzRE::JzEntity entity)
{
    auto it = m_entityWidgetMap.find(entity);
    if (it != m_entityWidgetMap.end()) {
        SelectActorByWidget(*it->second);
    }
}

void JzRE::JzHierarchy::CreateEmptyEntity()
{
    if (!JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &world  = JzServiceContainer::Get<JzWorld>();
    auto  entity = world.CreateEntity();

    // Generate unique name
    String entityName = "Entity_" + std::to_string(m_entityCounter++);

    // Add basic components
    world.AddComponent<JzNameComponent>(entity, entityName);
    world.AddComponent<JzTransformComponent>(entity);
    world.AddComponent<JzActiveTag>(entity);

    m_needsRefresh = true;
}

void JzRE::JzHierarchy::DeleteSelectedEntity()
{
    if (!JzServiceContainer::Has<JzEditorState>() || !JzServiceContainer::Has<JzWorld>()) {
        return;
    }

    auto &editorState = JzServiceContainer::Get<JzEditorState>();
    if (!editorState.HasSelection()) {
        return;
    }

    auto &world = JzServiceContainer::Get<JzWorld>();

    // Detach assets before destroying
    if (JzServiceContainer::Has<JzAssetSystem>()) {
        auto &assetSystem = JzServiceContainer::Get<JzAssetSystem>();
        assetSystem.DetachAllAssets(world, editorState.selectedEntity);
    }

    world.DestroyEntity(editorState.selectedEntity);
    editorState.ClearSelection();
    SelectionClearedEvent.Invoke();

    m_needsRefresh = true;
}

void JzRE::JzHierarchy::AddModelFromFile()
{
    // Require project, world, and asset system
    if (!JzServiceContainer::Has<JzProjectManager>() || !JzServiceContainer::Get<JzProjectManager>().HasLoadedProject()) {
        return;
    }
    if (!JzServiceContainer::Has<JzWorld>() || !JzServiceContainer::Has<JzAssetSystem>()) {
        return;
    }

    auto      &projectManager = JzServiceContainer::Get<JzProjectManager>();
    const auto contentPath    = projectManager.GetContentPath();

    // Open file dialog for model selection
    JzOpenFileDialog dialog("Select Model File");
    auto             filters = JzAssetImporter::GetSupportedFileFilters();
    for (const auto &[label, filter] : filters) {
        dialog.AddFileType(label, filter);
    }
    dialog.AddFileType("All Files", "*.*");
    dialog.Show(JzEFileDialogType::OpenFile);

    if (!dialog.HasSucceeded()) {
        return;
    }

    std::filesystem::path selectedPath = dialog.GetSelectedFilePath();
    std::filesystem::path finalModelPath;

    // Check if the file is already inside the Content directory
    auto selectedCanonical = std::filesystem::weakly_canonical(selectedPath);
    auto contentCanonical  = std::filesystem::weakly_canonical(contentPath);

    bool isInsideContent = false;
    {
        auto selectedStr = selectedCanonical.string();
        auto contentStr  = contentCanonical.string();
        if (selectedStr.size() >= contentStr.size() && selectedStr.substr(0, contentStr.size()) == contentStr) {
            isInsideContent = true;
        }
    }

    if (isInsideContent) {
        // File is already in Content directory, use it directly
        finalModelPath = selectedPath;
    } else {
        // File is external, import it with dependencies
        if (!JzServiceContainer::Has<JzAssetImporter>()) {
            return;
        }
        auto &importer     = JzServiceContainer::Get<JzAssetImporter>();
        auto  importResult = importer.ImportModelWithDependencies(selectedPath);

        if (!importResult.allSucceeded || importResult.modelEntry.result != JzEImportResult::Success) {
            return;
        }

        finalModelPath = importResult.modelEntry.destinationPath;
    }

    auto &world       = JzServiceContainer::Get<JzWorld>();
    auto &assetSystem = JzServiceContainer::Get<JzAssetSystem>();

    // Load the model from the final path (inside Content)
    auto modelHandle = assetSystem.LoadSync<JzModel>(finalModelPath.string());
    if (!assetSystem.IsValid(modelHandle)) {
        return;
    }

    // Spawn entities from the model
    auto spawnedEntities = assetSystem.SpawnModel(world, modelHandle);

    // Add name and asset path components to spawned entities
    String baseName = finalModelPath.stem().string();

    // Store relative path from Content for serialization
    auto   relativePath    = std::filesystem::relative(finalModelPath, contentPath);
    String relativePathStr = relativePath.string();

    for (Size i = 0; i < spawnedEntities.size(); ++i) {
        auto entity = spawnedEntities[i];

        // Set name based on filename
        String entityName = baseName;
        if (spawnedEntities.size() > 1) {
            entityName += "_" + std::to_string(i);
        }

        if (world.HasComponent<JzNameComponent>(entity)) {
            world.GetComponent<JzNameComponent>(entity).name = entityName;
        } else {
            world.AddComponent<JzNameComponent>(entity, entityName);
        }

        // Add asset path for serialization (use relative path from Content)
        if (!world.HasComponent<JzAssetPathComponent>(entity)) {
            world.AddComponent<JzAssetPathComponent>(entity, relativePathStr);
        }

        // Add active tag
        if (!world.HasComponent<JzActiveTag>(entity)) {
            world.AddComponent<JzActiveTag>(entity);
        }
    }

    m_needsRefresh = true;
}
