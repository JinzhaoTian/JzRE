#include "JzWidget.h"

uint64_t JzRE::JzWidget::__WIDGET_ID_INCREMENT = 0;

JzRE::JzWidget::JzWidget()
{
    m_widgetID = "##" + std::to_string(__WIDGET_ID_INCREMENT++);
}

void JzRE::JzWidget::Draw()
{
    // TODO
}
