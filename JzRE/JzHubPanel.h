#pragma once

#include "CommonTypes.h"
#include "JzPanelWindow.h"
#include "JzButton.h"
#include "JzInputText.h"
#include "JzSpacing.h"
#include "JzSeparator.h"

namespace JzRE {
class JzHubPanel : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     */
    JzHubPanel();

    void Draw() override;

    std::optional<std::filesystem::path> GetResult() const;

private:
    void _UpdateGoButton(const String &p_path);
    void _OnFailedToOpenCorruptedProject(const std::filesystem::path &p_projectPath);
    void _OnFailedToCreateProject(const std::filesystem::path &p_projectPath);
    void _ShowError(const String &p_title, const String &p_message);
    Bool _ValidateResult(const std::filesystem::path &p_result);
    Bool _TryFinish(const std::filesystem::path p_result);

private:
    std::optional<std::filesystem::path> m_result;
    JzButton                            *m_goButton = nullptr;
};

} // namespace JzRE