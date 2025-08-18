#include "JzAssetBrowser.h"
#include "JzContext.h"
#include "JzOpenFileDialog.h"
#include "JzButton.h"
#include "JzSpacing.h"
#include "JzSeparator.h"

JzRE::JzAssetBrowser::JzAssetBrowser(const JzRE::String &name, JzRE::Bool is_opened) :
    JzPanelWindow(name, is_opened)
{
    AssetPathChangeEvent.AddListener([this] {
        Refresh();
    });

    auto &openButton                = CreateWidget<JzButton>("Open Folder");
    openButton.lineBreak            = false;
    openButton.idleBackgroundColor  = {0.7f, 0.5f, 0.0f};
    openButton.ClickedEvent        += [this] {
        JzOpenFileDialog dialog("Open Floder");
        dialog.AddFileType("*", "*.*");
        dialog.Show();

        const std::filesystem::path projectFile   = dialog.GetSelectedFilePath();
        const std::filesystem::path projectFolder = projectFile.parent_path();

        if (dialog.HasSucceeded()) {
            m_assetPath = projectFolder;
            AssetPathChangeEvent.Invoke();
        }
    };

    auto &refreshButton                = CreateWidget<JzButton>("Refresh");
    refreshButton.lineBreak            = true;
    refreshButton.idleBackgroundColor  = {0.f, 0.5f, 0.0f};
    refreshButton.ClickedEvent        += std::bind(&JzAssetBrowser::Refresh, this);

    CreateWidget<JzSpacing>(2);
    CreateWidget<JzSeparator>();
    CreateWidget<JzSpacing>(2);

    m_assetList = &CreateWidget<JzGroup>();
}

void JzRE::JzAssetBrowser::Fill()
{
    ConsiderItem(nullptr, std::filesystem::directory_entry(m_assetPath), true);
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

void JzRE::JzAssetBrowser::ParseFolder(JzRE::JzTreeNode &p_root, const std::filesystem::directory_entry &p_directory, JzRE::Bool p_isEngineItem, JzRE::Bool p_scriptFolder)
{
    // Iterates another time to display list files
    for (auto &item : std::filesystem::directory_iterator(p_directory)) {
        if (item.is_directory()) {
            ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
        }
    }

    // Iterates another time to display list files
    for (auto &item : std::filesystem::directory_iterator(p_directory)) {
        if (!item.is_directory()) {
            ConsiderItem(&p_root, item, p_isEngineItem, false, p_scriptFolder);
        }
    }
}

void JzRE::JzAssetBrowser::ConsiderItem(JzRE::JzTreeNode *p_root, const std::filesystem::directory_entry &p_entry, JzRE::Bool p_isEngineItem, JzRE::Bool p_autoOpen, JzRE::Bool p_scriptFolder)
{
    const Bool   isDirectory = p_entry.is_directory();
    const String itemname    = JzPathParser::GetElementName(p_entry.path().string());
    const auto   fileType    = JzPathParser::GetFileType(itemname);

    // Unknown file, so we skip it
    if (!isDirectory && fileType == JzEFileType::UNKNOWN) {
        return;
    }

    const String path = p_entry.path().string();

    const String resourceFormatPath = ""; // TODO
    const Bool   protectedItem      = !p_root || p_isEngineItem;

    /* If there is a given treenode (p_root) we attach the new widget to it */
    auto &itemGroup = p_root ? p_root->CreateWidget<JzGroup>() : m_assetList->CreateWidget<JzGroup>();

    /* Find the icon to apply to the item */
    const U32 iconTextureID = 0 /*isDirectory ? JzRE_CONTEXT().editorResources->GetTexture("Folder")->GetTexture().GetID() : JzRE_CONTEXT().editorResources->GetFileIcon(itemname)->GetTexture().GetID()*/;

    itemGroup.CreateWidget<JzImage>(iconTextureID, JzVec2{16, 16}).lineBreak = false;

    /* If the entry is a directory, the content must be a tree node, otherwise (= is a file), a text will suffice */
    if (isDirectory) {
        auto &treeNode = itemGroup.CreateWidget<JzTreeNode>(itemname);

        if (p_autoOpen) {
            treeNode.Open();
        }

        treeNode.OpenedEvent += [this, &treeNode, path, p_isEngineItem, p_scriptFolder] {
            treeNode.RemoveAllWidgets();
            String updatedPath = JzPathParser::GetContainingFolder(path) + treeNode.name;
            ParseFolder(treeNode, std::filesystem::directory_entry(updatedPath), p_isEngineItem, p_scriptFolder);
        };

        treeNode.ClosedEvent += [this, &treeNode] {
            treeNode.RemoveAllWidgets();
        };
    } else {
        auto &clickableText = itemGroup.CreateWidget<JzTextClickable>(itemname);

        if (fileType == JzEFileType::MATERIAL) {
            clickableText.DoubleClickedEvent += [p_isEngineItem] { };
        }
    }
}