/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Runtime/Core/JzServiceContainer.h"
#include "JzRE/Editor/Panels/JzAssetBrowser.h"
#include "JzRE/Editor/UI/JzButton.h"
#include "JzRE/Editor/UI/JzFileContextMenu.h"
#include "JzRE/Editor/UI/JzFolderContextMenu.h"
#include "JzRE/Editor/UI/JzIcon.h"
#include "JzRE/Editor/UI/JzSeparator.h"
#include "JzRE/Editor/UI/JzTextClickable.h"
#include "JzRE/Runtime/Resource/JzAssetManager.h"
#include "JzRE/Runtime/Resource/JzTexture.h"

JzRE::JzAssetBrowser::JzAssetBrowser(const JzRE::String &name, JzRE::Bool is_opened) :
    JzPanelWindow(name, is_opened)
{
    m_openDirectory = std::filesystem::current_path();

    auto &refreshButton             = CreateWidget<JzButton>("Refresh");
    refreshButton.buttonIdleColor   = "#e3c79f";
    refreshButton.buttonLabelColor  = "#003153";
    refreshButton.lineBreak         = false;
    refreshButton.ClickedEvent     += [this]() { Refresh(); };

    auto &importButton             = CreateWidget<JzButton>("Import");
    importButton.buttonIdleColor   = "#b5120f";
    importButton.buttonLabelColor  = "#003153";
    importButton.lineBreak         = true;
    importButton.ClickedEvent     += []() {
        // TODO
    };

    CreateWidget<JzSeparator>();

    m_assetList = &CreateWidget<JzGroup>();

    Fill();
}

void JzRE::JzAssetBrowser::Fill()
{
    for (const auto &item : std::filesystem::directory_iterator(m_openDirectory.string())) {
        try {
            if (item.is_directory()) {
                _AddDirectoryItem(nullptr, item, true);
            }
        } catch (const std::exception &ex) {
            JzRE_LOG_ERROR("Failed to process item: {} - {}", item.path().string(), ex.what());
        }
    }
}

void JzRE::JzAssetBrowser::Clear()
{
    m_assetList->RemoveAllWidgets();
}

void JzRE::JzAssetBrowser::Refresh()
{
    Clear();
    Fill();
}

void JzRE::JzAssetBrowser::_TraverseDirectory(JzRE::JzTreeNode &root, const std::filesystem::path &path)
{
    for (auto &item : std::filesystem::directory_iterator(path)) {
        if (item.is_directory()) {
            _AddDirectoryItem(&root, item, false);
        }
    }

    for (auto &item : std::filesystem::directory_iterator(path)) {
        if (!item.is_directory()) {
            _AddFileItem(&root, item);
        }
    }
}

void JzRE::JzAssetBrowser::_AddDirectoryItem(JzRE::JzTreeNode *root, const std::filesystem::path &path, JzRE::Bool autoOpen)
{
    auto &itemGroup = root ? root->CreateWidget<JzGroup>() : m_assetList->CreateWidget<JzGroup>();

    const auto itemName = path.filename().string();

    /* Find the icon to apply to the item */
    auto      &assetManager = JzServiceContainer::Get<JzAssetManager>();
    auto       iconHandle   = assetManager.GetOrLoad<JzTexture>("icons/folder-16.png");
    auto       iconTexture  = assetManager.GetShared(iconHandle);

    auto &iconItem     = itemGroup.CreateWidget<JzIcon>(iconTexture->GetRhiTexture(), JzVec2{16, 16});
    iconItem.lineBreak = false;

    auto &treeNode        = itemGroup.CreateWidget<JzTreeNode>(itemName);
    treeNode.OpenedEvent += [this, &treeNode, path] {
        treeNode.RemoveAllWidgets();
        _TraverseDirectory(treeNode, path);
    };
    treeNode.ClosedEvent += [this, &treeNode] {
        treeNode.RemoveAllWidgets();
    };

    auto &contextMenu = treeNode.AddPlugin<JzFolderContextMenu>(path.string());
    contextMenu.CreateList();
    contextMenu.ItemAddedEvent += [this, &treeNode](std::filesystem::path p_path) {
        treeNode.Open();
        treeNode.RemoveAllWidgets();
        _TraverseDirectory(treeNode, std::filesystem::directory_entry(p_path.parent_path()));
    };

    if (autoOpen) {
        treeNode.Open();
    }
}

void JzRE::JzAssetBrowser::_AddFileItem(JzRE::JzTreeNode *root, const std::filesystem::path &path)
{
    auto &itemGroup = root ? root->CreateWidget<JzGroup>() : m_assetList->CreateWidget<JzGroup>();

    const auto itemName = path.filename().string();

    /* Find the icon to apply to the item */
    auto      &assetManager = JzServiceContainer::Get<JzAssetManager>();
    auto       iconHandle   = assetManager.GetOrLoad<JzTexture>("icons/file-16.png");
    auto       iconTexture  = assetManager.GetShared(iconHandle);

    auto &iconItem     = itemGroup.CreateWidget<JzIcon>(iconTexture->GetRhiTexture(), JzVec2{16, 16});
    iconItem.lineBreak = false;

    auto &clickableText = itemGroup.CreateWidget<JzTextClickable>(itemName);

    auto &contextMenu = clickableText.AddPlugin<JzFileContextMenu>(path.string());
    contextMenu.CreateList();
    contextMenu.DestroyedEvent += [&itemGroup](std::filesystem::path p_deletedPath) { };
    contextMenu.RenamedEvent   += [&clickableText](std::filesystem::path p_prev, std::filesystem::path p_newPath) { };
    contextMenu.DuplicateEvent += [this, &clickableText](std::filesystem::path newItem) { };

    clickableText.DoubleClickedEvent += []() { };
}