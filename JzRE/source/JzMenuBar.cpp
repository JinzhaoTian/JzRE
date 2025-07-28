#include "JzMenuBar.h"
#include "JzEditorActions.h"

JzRE::JzMenuBar::JzMenuBar()
{
    CreateFileMenu();
    CreateBuildMenu();
    CreateWindowMenu();
    CreateActorsMenu();
    CreateResourcesMenu();
    CreateToolsMenu();
    CreateSettingsMenu();
    CreateLayoutMenu();
    CreateHelpMenu();
}

void JzRE::JzMenuBar::HandleShortcuts(F32 deltaTime) { }

void JzRE::JzMenuBar::RegisterPanel(const String &name, JzPanelWindow &panel) { }

void JzRE::JzMenuBar::InitializeSettingsMenu() { }

void JzRE::JzMenuBar::CreateFileMenu()
{
    auto &fileMenu                                                           = CreateWidget<JzMenuList>("File");
    fileMenu.CreateWidget<JzMenuItem>("New Scene", "CTRL + N").ClickedEvent += EDITOR_BIND(LoadEmptyScene);
    fileMenu.CreateWidget<JzMenuItem>("Exit", "ALT + F4").ClickedEvent      += [] { EDITOR_CONTEXT(window)->SetShouldClose(true); };
}

void JzRE::JzMenuBar::CreateBuildMenu() { }

void JzRE::JzMenuBar::CreateWindowMenu() { }

void JzRE::JzMenuBar::CreateActorsMenu() { }

void JzRE::JzMenuBar::CreateResourcesMenu() { }

void JzRE::JzMenuBar::CreateToolsMenu() { }

void JzRE::JzMenuBar::CreateSettingsMenu() { }

void JzRE::JzMenuBar::CreateLayoutMenu() { }

void JzRE::JzMenuBar::CreateHelpMenu()
{
    auto &helpMenu = CreateWidget<JzMenuList>("Help");
    helpMenu.CreateWidget<JzText>("JzRE: " + std::string("test"));
}

void JzRE::JzMenuBar::UpdateToggleableItems() { }

void JzRE::JzMenuBar::OpenEveryWindows(JzRE::Bool state) { }