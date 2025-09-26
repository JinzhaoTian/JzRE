/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzFileDialog.h"

#ifdef __APPLE__

#include <Cocoa/Cocoa.h>
#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <filesystem>

JzRE::JzFileDialog::JzFileDialog(const JzRE::String &p_dialogTitle)
    : m_dialogTitle(p_dialogTitle), m_initialDirectory(""), m_succeeded(false) {
}

void JzRE::JzFileDialog::SetInitialDirectory(
    const JzRE::String &p_initialDirectory) {
  m_initialDirectory = p_initialDirectory;
}

void JzRE::JzFileDialog::Show(JzEFileDialogType type) {
  m_succeeded = false;
  m_filepath.clear();
  m_filename.clear();

  @autoreleasepool {
    NSOpenPanel *openPanel = [NSOpenPanel openPanel];

    [openPanel setTitle:@(m_dialogTitle.c_str())];
    switch (type) {
    case JzRE::JzEFileDialogType::DEFAULT:
      [openPanel setCanChooseFiles:YES];
      [openPanel setCanChooseDirectories:YES];
      break;
    case JzRE::JzEFileDialogType::OpenFile:
      [openPanel setCanChooseFiles:YES];
      [openPanel setCanChooseDirectories:NO];
      break;
    case JzRE::JzEFileDialogType::OpenFolder:
      [openPanel setCanChooseFiles:NO];
      [openPanel setCanChooseDirectories:YES];
      break;
    case JzRE::JzEFileDialogType::SaveFile:
    default:
      [openPanel setCanChooseFiles:YES];
      [openPanel setCanChooseDirectories:YES];
    }
    [openPanel setAllowsMultipleSelection:NO];

    // Set initial directory
    if (!m_initialDirectory.empty()) {
      NSURL *directoryURL =
          [NSURL fileURLWithPath:@(m_initialDirectory.c_str())];
      [openPanel setDirectoryURL:directoryURL];
    }

    // Parse and set file types
    if (!m_filter.empty()) {
      NSMutableArray *allowedTypes = [[NSMutableArray alloc] init];
      std::string filter = m_filter;
      size_t pos = 0;

      while ((pos = filter.find("|")) != std::string::npos) {
        std::string item = filter.substr(0, pos);
        size_t colonPos = item.find(":");
        if (colonPos != std::string::npos) {
          std::string extensions = item.substr(colonPos + 1);
          // Parse extensions (e.g., "*.txt;*.doc" -> "txt", "doc")
          size_t extPos = 0;
          while ((extPos = extensions.find("*.")) != std::string::npos) {
            extensions.erase(extPos, 2); // Remove "*."
            size_t semicolonPos = extensions.find(";");
            std::string ext = extensions.substr(0, semicolonPos);
            [allowedTypes addObject:@(ext.c_str())];
            if (semicolonPos != std::string::npos) {
              extensions = extensions.substr(semicolonPos + 1);
            } else {
              break;
            }
          }
        }
        filter.erase(0, pos + 1);
      }

      if (allowedTypes.count > 0) {
        // Use the new API for macOS 12.0+, fallback to deprecated API for older
        // versions
        if (@available(macOS 12.0, *)) {
          NSMutableArray *contentTypes = [[NSMutableArray alloc] init];
          for (NSString *ext in allowedTypes) {
            UTType *type = [UTType typeWithFilenameExtension:ext];
            if (type) {
              [contentTypes addObject:type];
            }
          }
          if (contentTypes.count > 0) {
            [openPanel setAllowedContentTypes:contentTypes];
          }
        } else {
// Fallback for older macOS versions
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
          [openPanel setAllowedFileTypes:allowedTypes];
#pragma clang diagnostic pop
        }
      }
    }

    NSInteger result = [openPanel runModal];
    if (result == NSModalResponseOK) {
      NSURL *selectedFile = [[openPanel URLs] objectAtIndex:0];
      NSString *path = [selectedFile path];
      m_filepath = [path UTF8String];
      m_succeeded = true;
    }
  }

  if (m_succeeded) {
    m_filename.clear();
    if (!m_filepath.empty()) {
      for (auto it = m_filepath.rbegin();
           it != m_filepath.rend() && *it != '\\' && *it != '/'; ++it)
        m_filename += *it;
      std::reverse(m_filename.begin(), m_filename.end());
    }
  }
}

JzRE::Bool JzRE::JzFileDialog::HasSucceeded() const { return m_succeeded; }

JzRE::String JzRE::JzFileDialog::GetSelectedFileName() { return m_filename; }

JzRE::String JzRE::JzFileDialog::GetSelectedFilePath() { return m_filepath; }

JzRE::String JzRE::JzFileDialog::GetErrorInfo() { return m_error; }

JzRE::Bool JzRE::JzFileDialog::IsFileExisting() const {
  return std::filesystem::exists(m_filepath);
}

void JzRE::JzFileDialog::HandleError() {
  // TODO: Implement proper error handling
  m_error = "file dialog error occurred";
}

#endif // __APPLE__
