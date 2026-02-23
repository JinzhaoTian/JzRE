/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <memory>
#include "JzRE/Runtime/Core/JzRETypes.h"
#include "JzRE/Editor/Core/JzEvent.h"
#include "JzRE/Runtime/Platform/RHI/JzGPUTextureObject.h"
#include "JzRE/Editor/UI/JzDataWidget.h"
#include "JzRE/Editor/UI/JzWidgetContainer.h"

namespace JzRE {

/**
 * @brief Icon tree node
 */
class JzIconTreeNode : public JzDataWidget<String>, public JzWidgetContainer {
public:
    /**
     * @brief Constructor
     *
     * @param name
     * @param iconTexture
     * @param arrowClickToOpen
     */
    JzIconTreeNode(const String &name, std::shared_ptr<JzGPUTextureObject> iconTexture, Bool arrowClickToOpen = false);

    /**
     * @brief Open the tree node
     */
    void Open();

    /**
     * @brief Close the tree node
     */
    void Close();

    /**
     * @brief Check if the tree node is opened
     *
     * @return True if the tree node is opened, false otherwise
     */
    Bool IsOpened() const;

protected:
    /**
     * @brief Draw Implement
     */
    virtual void _Draw_Impl() override;

public:
    String    name;
    Bool      selected = false;
    Bool      leaf     = false;
    JzEvent<> ClickedEvent;
    JzEvent<> DoubleClickedEvent;
    JzEvent<> OpenedEvent;
    JzEvent<> ClosedEvent;

private:
    std::shared_ptr<JzGPUTextureObject> m_iconTexture;
    Bool                                m_arrowClickToOpen = false;
    Bool                                m_shouldOpen       = false;
    Bool                                m_shouldClose      = false;
    Bool                                m_opened           = false;
};

} // namespace JzRE
