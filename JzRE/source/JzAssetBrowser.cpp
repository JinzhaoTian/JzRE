#include "JzAssetBrowser.h"

JzRE::JzAssetBrowser::JzAssetBrowser(const JzRE::String &name, JzRE::Bool is_opened) :
    JzPanelWindow(name, is_opened)
{
    auto &refreshButton         = CreateWidget<JzButton>("Rescan assets");
    refreshButton.ClickedEvent += std::bind(&JzAssetBrowser::Refresh, this);
    refreshButton.lineBreak     = false;

    auto &importButton = CreateWidget<JzButton>("Import asset");

    m_assetList = &CreateWidget<JzGroup>();

    Fill();
}

void JzRE::JzAssetBrowser::Fill()
{
    // to fill fake data
    m_assetList->CreateWidget<JzSeparator>();

    m_assetList->CreateWidget<JzSeparator>();

    m_assetList->CreateWidget<JzSeparator>();
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