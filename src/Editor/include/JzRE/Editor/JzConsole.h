/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzELog.h"
#include "JzRE/Editor/UI/JzPanelWindow.h"
#include "JzRE/Editor/UI/JzGroup.h"
#include "JzRE/Editor/UI/JzText.h"

namespace JzRE {
/**
 * @brief Console Panel Window
 */
class JzConsole : public JzPanelWindow {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param is_opened
     */
    JzConsole(const String &name, Bool is_opened);

    /**
     * @brief Clear Content
     */
    void Clear();

    /**
     * @brief Filter logs
     */
    void FilterLogs();

    /**
     * @brief
     */
    void TruncateLogs();

private:
    void OnLogMessage(const JzLogMessage &msg);
    void SetShowDefaultLogs(Bool value);
    void SetShowInfoLogs(Bool value);
    void SetShowWarningLogs(Bool value);
    void SetShowErrorLogs(Bool value);
    Bool IsAllowedByFilter(JzELogLevel level);

private:
    JzGroup                                  *m_logGroup;
    std::unordered_map<JzText *, JzELogLevel> m_logTextWidgets;
    Bool                                      m_clearOnPlay    = true;
    Bool                                      m_showDefaultLog = true;
    Bool                                      m_showInfoLog    = true;
    Bool                                      m_showWarningLog = true;
    Bool                                      m_showErrorLog   = true;
};
} // namespace JzRE