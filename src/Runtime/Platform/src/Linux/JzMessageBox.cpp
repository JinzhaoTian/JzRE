/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef __linux__

#include "JzRE/Runtime/Platform/Dialog/JzMessageBox.h"

#include <gtk/gtk.h>
#include <iostream>

namespace JzRE {

JzMessageBox::JzMessageBox(std::string title, std::string message,
                           JzEMessageBoxType type, JzEMessageBoxButton button, bool autoShow) :
    m_title(std::move(title)),
    m_message(std::move(message)),
    m_type(type),
    m_button(button),
    m_result(JzEMessageBoxUserAction::Cancel)
{
    if (autoShow)
        Show();
}

const JzEMessageBoxUserAction &JzMessageBox::GetUserAction() const
{
    return m_result;
}

// GTK回调函数
struct MessageBoxData {
    GtkWidget               *dialog;
    JzEMessageBoxUserAction *result;
};

static void on_response(GtkDialog *dialog, gint response_id, gpointer user_data)
{
    MessageBoxData *data = static_cast<MessageBoxData *>(user_data);

    switch (response_id) {
        case GTK_RESPONSE_OK:
            *data->result = JzEMessageBoxUserAction::OK;
            break;
        case GTK_RESPONSE_CANCEL:
            *data->result = JzEMessageBoxUserAction::Cancel;
            break;
        case GTK_RESPONSE_YES:
            *data->result = JzEMessageBoxUserAction::Yes;
            break;
        case GTK_RESPONSE_NO:
            *data->result = JzEMessageBoxUserAction::No;
            break;
        case GTK_RESPONSE_ACCEPT: // 作为CONTINUE
            *data->result = JzEMessageBoxUserAction::Continue;
            break;
        case GTK_RESPONSE_REJECT: // 作为IGNORE
            *data->result = JzEMessageBoxUserAction::Ignore;
            break;
        case GTK_RESPONSE_APPLY: // 作为RETRY
            *data->result = JzEMessageBoxUserAction::Retry;
            break;
        case GTK_RESPONSE_HELP: // 作为TRYAGAIN
            *data->result = JzEMessageBoxUserAction::Tryagain;
            break;
        default:
            *data->result = JzEMessageBoxUserAction::Cancel;
            break;
    }

    gtk_widget_destroy(data->dialog);
    g_free(data);
}

void JzMessageBox::Show()
{
    if (!gtk_init_check(nullptr, nullptr)) {
        std::cerr << "Failed to initialize GTK" << std::endl;
        return;
    }

    GtkMessageType gtk_type;
    switch (m_type) {
        case JzEMessageBoxType::Question:
            gtk_type = GTK_MESSAGE_QUESTION;
            break;
        case JzEMessageBoxType::Information:
            gtk_type = GTK_MESSAGE_INFO;
            break;
        case JzEMessageBoxType::Warning:
            gtk_type = GTK_MESSAGE_WARNING;
            break;
        case JzEMessageBoxType::Error:
            gtk_type = GTK_MESSAGE_ERROR;
            break;
        default:
            gtk_type = GTK_MESSAGE_INFO;
            break;
    }

    GtkButtonsType gtk_buttons;
    switch (m_button) {
        case JzEMessageBoxButton::OK:
            gtk_buttons = GTK_BUTTONS_OK;
            break;
        case JzEMessageBoxButton::OK_CANCEL:
            gtk_buttons = GTK_BUTTONS_OK_CANCEL;
            break;
        case JzEMessageBoxButton::YES_NO:
            gtk_buttons = GTK_BUTTONS_YES_NO;
            break;
        case JzEMessageBoxButton::YES_NO_CANCEL:
            gtk_buttons = GTK_BUTTONS_YES_NO;
            // 对于YES_NO_CANCEL，需要自定义按钮
            break;
        case JzEMessageBoxButton::RETRY_CANCEL:
            gtk_buttons = GTK_BUTTONS_NONE;
            break;
        case JzEMessageBoxButton::ABORT_RETRY_IGNORE:
            gtk_buttons = GTK_BUTTONS_NONE;
            break;
        case JzEMessageBoxButton::CANCEL_TRYAGAIN_CONTINUE:
            gtk_buttons = GTK_BUTTONS_NONE;
            break;
        case JzEMessageBoxButton::HELP:
            gtk_buttons = GTK_BUTTONS_OK;
            break;
        default:
            gtk_buttons = GTK_BUTTONS_OK;
            break;
    }

    GtkWidget *dialog = gtk_message_dialog_new(nullptr,
                                               GTK_DIALOG_MODAL,
                                               gtk_type,
                                               gtk_buttons,
                                               "%s",
                                               m_message.c_str());

    gtk_window_set_title(GTK_WINDOW(dialog), m_title.c_str());

    // 自定义按钮处理
    switch (m_button) {
        case JzEMessageBoxButton::YES_NO_CANCEL:
            gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                                   "_Yes", GTK_RESPONSE_YES,
                                   "_No", GTK_RESPONSE_NO,
                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                   nullptr);
            break;
        case JzEMessageBoxButton::RETRY_CANCEL:
            gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                                   "_Retry", GTK_RESPONSE_APPLY,
                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                   nullptr);
            break;
        case JzEMessageBoxButton::ABORT_RETRY_IGNORE:
            gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                                   "_Abort", GTK_RESPONSE_CANCEL,
                                   "_Retry", GTK_RESPONSE_APPLY,
                                   "_Ignore", GTK_RESPONSE_REJECT,
                                   nullptr);
            break;
        case JzEMessageBoxButton::CANCEL_TRYAGAIN_CONTINUE:
            gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                                   "_Cancel", GTK_RESPONSE_CANCEL,
                                   "_Try Again", GTK_RESPONSE_HELP,
                                   "_Continue", GTK_RESPONSE_ACCEPT,
                                   nullptr);
            break;
        case JzEMessageBoxButton::HELP:
            gtk_dialog_add_buttons(GTK_DIALOG(dialog),
                                   "_OK", GTK_RESPONSE_OK,
                                   "_Help", GTK_RESPONSE_HELP,
                                   nullptr);
            break;
        default:
            break;
    }

    MessageBoxData *data = g_new(MessageBoxData, 1);
    data->dialog         = dialog;
    data->result         = &m_result;

    g_signal_connect(dialog, "response", G_CALLBACK(on_response), data);
    gtk_widget_show_all(dialog);

    // 运行GTK主循环
    gtk_main();
}

} // namespace JzRE

#endif // __linux__