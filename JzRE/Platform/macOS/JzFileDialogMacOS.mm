#include "JzFileDialogMacOS.h"

#ifdef __APPLE__
#include <Cocoa/Cocoa.h>
#include <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#include <filesystem>

JzRE::JzFileDialogMacOS::JzFileDialogMacOS(const JzRE::String &p_dialogTitle) :
    m_dialogTitle(p_dialogTitle),
    m_initialDirectory(""),
    m_succeeded(false) { }

void JzRE::JzFileDialogMacOS::SetInitialDirectory(const JzRE::String &p_initialDirectory)
{
    m_initialDirectory = p_initialDirectory;
}

void JzRE::JzFileDialogMacOS::Show()
{
    m_succeeded = false;
    m_filepath.clear();
    m_filename.clear();

    @autoreleasepool {
        NSOpenPanel *openPanel = [NSOpenPanel openPanel];

        [openPanel setTitle:@(m_dialogTitle.c_str())];
        [openPanel setCanChooseFiles:YES];
        [openPanel setCanChooseDirectories:NO];
        [openPanel setAllowsMultipleSelection:NO];

        // Set initial directory
        if (!m_initialDirectory.empty()) {
            NSURL *directoryURL = [NSURL fileURLWithPath:@(m_initialDirectory.c_str())];
            [openPanel setDirectoryURL:directoryURL];
        }

        // Parse and set file types
        if (!m_filter.empty()) {
            NSMutableArray *allowedTypes = [[NSMutableArray alloc] init];
            std::string     filter       = m_filter;
            size_t          pos          = 0;

            while ((pos = filter.find("|")) != std::string::npos) {
                std::string item     = filter.substr(0, pos);
                size_t      colonPos = item.find(":");
                if (colonPos != std::string::npos) {
                    std::string extensions = item.substr(colonPos + 1);
                    // Parse extensions (e.g., "*.txt;*.doc" -> "txt", "doc")
                    size_t extPos = 0;
                    while ((extPos = extensions.find("*.")) != std::string::npos) {
                        extensions.erase(extPos, 2); // Remove "*."
                        size_t      semicolonPos = extensions.find(";");
                        std::string ext          = extensions.substr(0, semicolonPos);
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
                // Use the new API for macOS 12.0+, fallback to deprecated API for older versions
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
            NSURL    *selectedFile = [[openPanel URLs] objectAtIndex:0];
            NSString *path         = [selectedFile path];
            m_filepath             = [path UTF8String];
            m_succeeded            = true;
        }
    }

    if (m_succeeded) {
        m_filename.clear();
        if (!m_filepath.empty()) {
            for (auto it = m_filepath.rbegin(); it != m_filepath.rend() && *it != '\\' && *it != '/'; ++it)
                m_filename += *it;
            std::reverse(m_filename.begin(), m_filename.end());
        }
    }
}

JzRE::Bool JzRE::JzFileDialogMacOS::HasSucceeded() const
{
    return m_succeeded;
}

JzRE::String JzRE::JzFileDialogMacOS::GetSelectedFileName()
{
    return m_filename;
}

JzRE::String JzRE::JzFileDialogMacOS::GetSelectedFilePath()
{
    return m_filepath;
}

JzRE::String JzRE::JzFileDialogMacOS::GetErrorInfo()
{
    return m_error;
}

JzRE::Bool JzRE::JzFileDialogMacOS::IsFileExisting() const
{
    return std::filesystem::exists(m_filepath);
}

void JzRE::JzFileDialogMacOS::AddFileType(const JzRE::String &p_label, const JzRE::String &p_filter)
{
    if (!m_filter.empty()) {
        m_filter += "|";
    }
    m_filter += p_label + ":" + p_filter;
}

void JzRE::JzFileDialogMacOS::HandleError()
{
    // TODO: Implement proper error handling
    m_error = "macOS file dialog error occurred";
}

#endif // __APPLE__
