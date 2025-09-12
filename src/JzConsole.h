/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include "JzRETypes.h"
#include "JzGroup.h"
#include "JzPanelWindow.h"

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
    void SetShowDefaultLogs(Bool value);
    void SetShowInfoLogs(Bool value);
    void SetShowWarningLogs(Bool value);
    void SetShowErrorLogs(Bool value);

private:
    JzGroup *m_logGroup;
    Bool     m_clearOnPlay    = true;
    Bool     m_showDefaultLog = true;
    Bool     m_showInfoLog    = true;
    Bool     m_showWarningLog = true;
    Bool     m_showErrorLog   = true;
};
} // namespace JzRE