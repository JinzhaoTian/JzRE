/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef __APPLE__

#include "JzRE/Runtime/Platform/JzMessageBox.h"

#import <Cocoa/Cocoa.h>

namespace JzRE {

JzMessageBox::JzMessageBox(String title, String message, JzEMessageBoxType type,
                           JzEMessageBoxButton button, Bool autoShow)
    : m_title(std::move(title)), m_message(std::move(message)), m_type(type),
      m_button(button), m_result(JzEMessageBoxUserAction::Cancel) {
  if (autoShow)
    Show();
}

const JzEMessageBoxUserAction &JzMessageBox::GetUserAction() const {
  return m_result;
}

void JzMessageBox::Show() {
  // 确保在主线程中执行UI操作
  if (![NSThread isMainThread]) {
    dispatch_sync(dispatch_get_main_queue(), ^{
      Show();
    });
    return;
  }

  @autoreleasepool {
    NSString *nsTitle = [NSString stringWithUTF8String:m_title.c_str()];
    NSString *nsMessage = [NSString stringWithUTF8String:m_message.c_str()];

    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText:nsTitle];
    [alert setInformativeText:nsMessage];

    // 设置图标类型
    switch (m_type) {
    case JzEMessageBoxType::Question:
      [alert setAlertStyle:NSAlertStyleInformational];
      break;
    case JzEMessageBoxType::Information:
      [alert setAlertStyle:NSAlertStyleInformational];
      break;
    case JzEMessageBoxType::Warning:
      [alert setAlertStyle:NSAlertStyleWarning];
      break;
    case JzEMessageBoxType::Error:
      [alert setAlertStyle:NSAlertStyleCritical];
      break;
    }

    // 添加按钮
    switch (m_button) {
    case JzEMessageBoxButton::OK:
      [alert addButtonWithTitle:@"OK"];
      break;
    case JzEMessageBoxButton::OK_CANCEL:
      [alert addButtonWithTitle:@"OK"];
      [alert addButtonWithTitle:@"Cancel"];
      break;
    case JzEMessageBoxButton::YES_NO:
      [alert addButtonWithTitle:@"Yes"];
      [alert addButtonWithTitle:@"No"];
      break;
    case JzEMessageBoxButton::YES_NO_CANCEL:
      [alert addButtonWithTitle:@"Yes"];
      [alert addButtonWithTitle:@"No"];
      [alert addButtonWithTitle:@"Cancel"];
      break;
    case JzEMessageBoxButton::RETRY_CANCEL:
      [alert addButtonWithTitle:@"Retry"];
      [alert addButtonWithTitle:@"Cancel"];
      break;
    case JzEMessageBoxButton::ABORT_RETRY_IGNORE:
      [alert addButtonWithTitle:@"Abort"];
      [alert addButtonWithTitle:@"Retry"];
      [alert addButtonWithTitle:@"Ignore"];
      break;
    case JzEMessageBoxButton::CANCEL_TRYAGAIN_CONTINUE:
      [alert addButtonWithTitle:@"Cancel"];
      [alert addButtonWithTitle:@"Try Again"];
      [alert addButtonWithTitle:@"Continue"];
      break;
    case JzEMessageBoxButton::HELP:
      [alert addButtonWithTitle:@"OK"];
      [alert addButtonWithTitle:@"Help"];
      break;
    }

    NSInteger response = [alert runModal];

    // 映射响应到我们的枚举
    switch (response) {
    case NSAlertFirstButtonReturn:
      switch (m_button) {
      case JzEMessageBoxButton::OK:
      case JzEMessageBoxButton::OK_CANCEL:
      case JzEMessageBoxButton::HELP:
        m_result = JzEMessageBoxUserAction::OK;
        break;
      case JzEMessageBoxButton::YES_NO:
      case JzEMessageBoxButton::YES_NO_CANCEL:
        m_result = JzEMessageBoxUserAction::Yes;
        break;
      case JzEMessageBoxButton::RETRY_CANCEL:
        m_result = JzEMessageBoxUserAction::Retry;
        break;
      case JzEMessageBoxButton::ABORT_RETRY_IGNORE:
        m_result = JzEMessageBoxUserAction::Cancel;
        break;
      case JzEMessageBoxButton::CANCEL_TRYAGAIN_CONTINUE:
        m_result = JzEMessageBoxUserAction::Cancel;
        break;
      }
      break;

    case NSAlertSecondButtonReturn:
      switch (m_button) {
      case JzEMessageBoxButton::OK:
      case JzEMessageBoxButton::OK_CANCEL:
        m_result = JzEMessageBoxUserAction::Cancel;
        break;
      case JzEMessageBoxButton::YES_NO:
      case JzEMessageBoxButton::YES_NO_CANCEL:
        m_result = JzEMessageBoxUserAction::No;
        break;
      case JzEMessageBoxButton::RETRY_CANCEL:
        m_result = JzEMessageBoxUserAction::Cancel;
        break;
      case JzEMessageBoxButton::ABORT_RETRY_IGNORE:
        m_result = JzEMessageBoxUserAction::Retry;
        break;
      case JzEMessageBoxButton::CANCEL_TRYAGAIN_CONTINUE:
        m_result = JzEMessageBoxUserAction::Tryagain;
        break;
      case JzEMessageBoxButton::HELP:
        m_result = JzEMessageBoxUserAction::Help;
        break;
      }
      break;

    case NSAlertThirdButtonReturn:
      switch (m_button) {
      case JzEMessageBoxButton::YES_NO_CANCEL:
        m_result = JzEMessageBoxUserAction::Cancel;
        break;
      case JzEMessageBoxButton::ABORT_RETRY_IGNORE:
        m_result = JzEMessageBoxUserAction::Ignore;
        break;
      case JzEMessageBoxButton::CANCEL_TRYAGAIN_CONTINUE:
        m_result = JzEMessageBoxUserAction::Continue;
        break;
      default:
        m_result = JzEMessageBoxUserAction::Cancel;
        break;
      }
      break;

    default:
      m_result = JzEMessageBoxUserAction::Cancel;
      break;
    }
  }
}

} // namespace JzRE

#endif // __APPLE__