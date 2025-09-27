/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Core/JzServiceContainer.h"
#include "JzRE/Editor/JzAssetBrowser.h"
#include "JzRE/Editor/JzContext.h"
#include "JzRE/Editor/JzPathParser.h"
#include "JzRE/UI/JzButton.h"
#include "JzRE/UI/JzFileContextMenu.h"
#include "JzRE/UI/JzFolderContextMenu.h"
#include "JzRE/UI/JzIcon.h"
#include "JzRE/UI/JzSeparator.h"
#include "JzRE/UI/JzTextClickable.h"
#include "JzRE/Resource/JzResourceManager.h"
#include "JzRE/Resource/JzTexture.h"

JzRE::JzAssetBrowser::JzAssetBrowser(const JzRE::String &name, JzRE::Bool is_opened) :
    JzPanelWindow(name, is_opened)
{
    m_openDirectory = JzContext::GetInstance().GetCurrentPath();

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

    m_assetList = &CreateWidget<JzGroup>();

    Fill();
}

void JzRE::JzAssetBrowser::Fill()
{
    m_assetList->CreateWidget<JzSeparator>();
    ConsiderItem(nullptr, std::filesystem::directory_entry(m_openDirectory), true);
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

void JzRE::JzAssetBrowser::ParseFolder(JzRE::JzTreeNode &root, const std::filesystem::directory_entry &directory)
{
    // Iterates another time to display list files
    for (auto &item : std::filesystem::directory_iterator(directory)) {
        if (item.is_directory()) {
            ConsiderItem(&root, item, false);
        }
    }

    // Iterates another time to display list files
    for (auto &item : std::filesystem::directory_iterator(directory)) {
        if (!item.is_directory()) {
            ConsiderItem(&root, item, false);
        }
    }
}

void JzRE::JzAssetBrowser::ConsiderItem(JzRE::JzTreeNode *root, const std::filesystem::directory_entry &pathEntry, JzRE::Bool autoOpen)
{
    const Bool   isDirectory = pathEntry.is_directory();
    const String itemName    = JzPathParser::GetElementName(pathEntry.path().string());
    const auto   fileType    = JzPathParser::GetFileType(itemName);

    // Unknown file, so we skip it
    if (!isDirectory && fileType == JzEFileType::UNKNOWN) {
        return;
    }

    const String path = pathEntry.path().string();

    const String resourceFormatPath = ""; // TODO

    /* If there is a given treenode (p_root) we attach the new widget to it */
    auto &itemGroup = root ? root->CreateWidget<JzGroup>() : m_assetList->CreateWidget<JzGroup>();

    /* Find the icon to apply to the item */
    auto      &resourceManager = JzServiceContainer::Get<JzResourceManager>();
    const auto iconTexture     = isDirectory ? resourceManager.GetResource<JzTexture>("icons/folder.png") : resourceManager.GetResource<JzTexture>("icons/file.png");

    auto &iconItem     = itemGroup.CreateWidget<JzIcon>(iconTexture->GetRhiTexture(), JzVec2{16, 16});
    iconItem.lineBreak = false;

    if (isDirectory) {
        auto &treeNode        = itemGroup.CreateWidget<JzTreeNode>(itemName);
        treeNode.OpenedEvent += [this, &treeNode, path] {
            treeNode.RemoveAllWidgets();
            String updatedPath = JzPathParser::GetContainingFolder(path) + treeNode.name;
            ParseFolder(treeNode, std::filesystem::directory_entry(updatedPath));
        };
        treeNode.ClosedEvent += [this, &treeNode] {
            treeNode.RemoveAllWidgets();
        };

        auto &contextMenu           = treeNode.AddPlugin<JzFolderContextMenu>(path);
        contextMenu.ItemAddedEvent += [this, &treeNode](std::filesystem::path p_path) {
            treeNode.Open();
            treeNode.RemoveAllWidgets();
            ParseFolder(
                treeNode,
                std::filesystem::directory_entry(p_path.parent_path()));
        };

        if (autoOpen) {
            treeNode.Open();
        }
    } else {
        auto &clickableText = itemGroup.CreateWidget<JzTextClickable>(itemName);

        auto &contextMenu = clickableText.AddPlugin<JzFileContextMenu>(path);
        contextMenu.CreateList();
        contextMenu.DestroyedEvent += [&itemGroup](std::filesystem::path p_deletedPath) { };
        contextMenu.RenamedEvent   += [&clickableText](std::filesystem::path p_prev, std::filesystem::path p_newPath) { };
        contextMenu.DuplicateEvent += [this, &clickableText](std::filesystem::path newItem) { };

        clickableText.DoubleClickedEvent += []() { };
    }
}