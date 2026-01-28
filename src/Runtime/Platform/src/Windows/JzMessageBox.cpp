/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef _WIN32

#include "JzRE/Runtime/Platform/Dialog/JzMessageBox.h"

#define NOMINMAX
#include <windows.h>

namespace JzRE {

JzMessageBox::JzMessageBox(String title, String message, JzEMessageBoxType type, JzEMessageBoxButton button, Bool autoShow) :
    m_title(std::move(title)),
    m_message(std::move(message)),
    m_type(type),
    m_button(button),
    m_result(JzEMessageBoxUserAction::Cancel)
{
    if (autoShow)
        Show();
}

void JzMessageBox::Show()
{
    UINT message_box_type = 0;
    switch (m_type) {
        case JzEMessageBoxType::Question:
            message_box_type |= MB_ICONQUESTION;
            break;
        case JzEMessageBoxType::Information:
            message_box_type |= MB_ICONINFORMATION;
            break;
        case JzEMessageBoxType::Warning:
            message_box_type |= MB_ICONWARNING;
            break;
        case JzEMessageBoxType::Error:
            message_box_type |= MB_ICONERROR;
            break;
    }

    UINT button_layout = 0;
    switch (m_button) {
        case JzEMessageBoxButton::OK:
            button_layout = MB_OK;
            break;
        case JzEMessageBoxButton::OK_CANCEL:
            button_layout = MB_OKCANCEL;
            break;
        case JzEMessageBoxButton::YES_NO:
            button_layout = MB_YESNO;
            break;
        case JzEMessageBoxButton::YES_NO_CANCEL:
            button_layout = MB_YESNOCANCEL;
            break;
        case JzEMessageBoxButton::RETRY_CANCEL:
            button_layout = MB_RETRYCANCEL;
            break;
        case JzEMessageBoxButton::ABORT_RETRY_IGNORE:
            button_layout = MB_ABORTRETRYIGNORE;
            break;
        case JzEMessageBoxButton::CANCEL_TRYAGAIN_CONTINUE:
            button_layout = MB_CANCELTRYCONTINUE;
            break;
        case JzEMessageBoxButton::HELP:
            button_layout = MB_OK | MB_HELP;
            break;
    }

    int msgbox_id = MessageBoxA(
        nullptr,
        m_message.c_str(),
        m_title.c_str(),
        message_box_type | button_layout | MB_DEFBUTTON2);

    switch (msgbox_id) {
        case IDOK:
            m_result = JzEMessageBoxUserAction::OK;
            break;
        case IDCANCEL:
            m_result = JzEMessageBoxUserAction::Cancel;
            break;
        case IDYES:
            m_result = JzEMessageBoxUserAction::Yes;
            break;
        case IDNO:
            m_result = JzEMessageBoxUserAction::No;
            break;
        case IDCONTINUE:
            m_result = JzEMessageBoxUserAction::Continue;
            break;
        case IDIGNORE:
            m_result = JzEMessageBoxUserAction::Ignore;
            break;
        case IDRETRY:
            m_result = JzEMessageBoxUserAction::Retry;
            break;
        case IDTRYAGAIN:
            m_result = JzEMessageBoxUserAction::Tryagain;
            break;
        default:
            m_result = JzEMessageBoxUserAction::Cancel;
            break;
    }
}

const JzEMessageBoxUserAction &JzMessageBox::GetUserAction() const
{
    return m_result;
}

} // namespace JzRE

#endif // _WIN32