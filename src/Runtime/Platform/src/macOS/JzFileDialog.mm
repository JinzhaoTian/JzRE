/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#ifdef __APPLE__

#include "JzRE/Runtime/Platform/Dialog/JzFileDialog.h"

#include <Cocoa/Cocoa.h>
#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>

JzRE::JzFileDialog::JzFileDialog(const JzRE::String &p_dialogTitle)
    : m_dialogTitle(p_dialogTitle), m_succeeded(false) {}

void JzRE::JzFileDialog::SetInitialDirectory(
    const std::filesystem::path &p_initialDirectory) {
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
    auto filterEntries = this->ParseFilters();
    if (!filterEntries.empty()) {
      NSMutableArray *allowedTypes = [[NSMutableArray alloc] init];

      for (const auto &entry : filterEntries) {
        for (const auto &ext : entry.extensions) {
          [allowedTypes addObject:@(ext.c_str())];
        }
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
    if (!m_filepath.empty()) {
      m_filename = m_filepath.filename();
    }
  }
}

JzRE::Bool JzRE::JzFileDialog::HasSucceeded() const { return m_succeeded; }

std::filesystem::path JzRE::JzFileDialog::GetSelectedFileName() {
  return m_filename;
}

std::filesystem::path JzRE::JzFileDialog::GetSelectedFilePath() {
  return m_filepath;
}

JzRE::String JzRE::JzFileDialog::GetErrorInfo() { return m_error; }

JzRE::Bool JzRE::JzFileDialog::IsFileExisting() const {
  return std::filesystem::exists(m_filepath);
}

void JzRE::JzFileDialog::HandleError() {
  // TODO: Implement proper error handling
  m_error = "file dialog error occurred";
}

#endif // __APPLE__
