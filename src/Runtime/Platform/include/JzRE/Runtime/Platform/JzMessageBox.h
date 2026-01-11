/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Enums of message box types
 */
enum class JzEMessageBoxType : U8 {
    Question,
    Information,
    Warning,
    Error
};

/**
 * @brief Enums of message box button action types
 */
enum class JzEMessageBoxButton : U8 {
    OK,
    OK_CANCEL,
    YES_NO,
    YES_NO_CANCEL,
    RETRY_CANCEL,
    ABORT_RETRY_IGNORE,
    CANCEL_TRYAGAIN_CONTINUE,
    HELP
};

/**
 * @brief Enums of message box user action result
 */
enum class JzEMessageBoxUserAction : U8 {
    OK,
    Cancel,
    Yes,
    No,
    Continue,
    Ignore,
    Retry,
    Tryagain,
    Help
};

/**
 * @brief Message Box
 */
class JzMessageBox {
public:
    /**
     * @brief Constructor
     *
     * @param title
     * @param message
     * @param type
     * @param button
     * @param autoShow
     */
    JzMessageBox(String title, String message, JzEMessageBoxType type = JzEMessageBoxType::Information, JzEMessageBoxButton button = JzEMessageBoxButton::OK, Bool autoShow = true);

    /**
     * @brief Show
     */
    void Show();

    /**
     * @brief Get the User Action
     *
     * @return const JzEMessageBoxUserAction&
     */
    const JzEMessageBoxUserAction &GetUserAction() const;

private:
    String                  m_title;
    String                  m_message;
    JzEMessageBoxType       m_type;
    JzEMessageBoxButton     m_button;
    JzEMessageBoxUserAction m_result;
};

} // namespace JzRE