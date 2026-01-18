/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Editor/Panels/JzConsole.h"
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Runtime/Core/JzLogger.h"
#include "JzRE/Editor/UI/JzText.h"
#include "JzRE/Editor/UI/JzButton.h"
#include "JzRE/Editor/UI/JzSeparator.h"
#include "JzRE/Editor/UI/JzSpacing.h"

JzRE::JzConsole::JzConsole(const String &name, Bool is_opened) :
    JzPanelWindow(name, is_opened)
{
    allowHorizontalScrollbar = true;

    auto &clearButton         = CreateWidget<JzButton>("Clear");
    clearButton.ClickedEvent += std::bind(&JzConsole::Clear, this);
    clearButton.lineBreak     = true;

    CreateWidget<JzSpacing>();
    CreateWidget<JzSeparator>();
    CreateWidget<JzSpacing>();

    m_logGroup = &CreateWidget<JzGroup>();
    m_logGroup->ReverseDrawOrder();

    JzLogger::GetInstance().OnLogMessage += std::bind(&JzConsole::OnLogMessage, this, std::placeholders::_1);
}

void JzRE::JzConsole::OnLogMessage(const JzLogMessage &msg)
{
    auto &text = m_logGroup->CreateWidget<JzText>(msg.message);
    // text.color              = msg.color;
    m_logTextWidgets[&text] = msg.level;
    text.enabled            = IsAllowedByFilter(msg.level);
}

void JzRE::JzConsole::Clear()
{
    m_logTextWidgets.clear();
    m_logGroup->RemoveAllWidgets();
}

void JzRE::JzConsole::FilterLogs()
{
    for (const auto &[widget, logLevel] : m_logTextWidgets)
        widget->enabled = IsAllowedByFilter(logLevel);
}

void JzRE::JzConsole::TruncateLogs()
{
    while (m_logGroup->GetWidgets().size() > 500) {
        m_logGroup->RemoveWidget(*m_logGroup->GetWidgets().front().first);
    }
}

void JzRE::JzConsole::SetShowDefaultLogs(JzRE::Bool value)
{
    m_showDefaultLog = value;
    FilterLogs();
}

void JzRE::JzConsole::SetShowInfoLogs(JzRE::Bool value)
{
    m_showInfoLog = value;
    FilterLogs();
}

void JzRE::JzConsole::SetShowWarningLogs(JzRE::Bool value)
{
    m_showWarningLog = value;
    FilterLogs();
}

void JzRE::JzConsole::SetShowErrorLogs(JzRE::Bool value)
{
    m_showErrorLog = value;
    FilterLogs();
}

JzRE::Bool JzRE::JzConsole::IsAllowedByFilter(JzELogLevel level)
{
    switch (level) {
        case JzELogLevel::Trace:
        case JzELogLevel::Debug:
        case JzELogLevel::Critical:
            return m_showDefaultLog;
        case JzELogLevel::Info:
            return m_showInfoLog;
        case JzELogLevel::Warning:
            return m_showWarningLog;
        case JzELogLevel::Error:
            return m_showErrorLog;
    }

    return false;
}