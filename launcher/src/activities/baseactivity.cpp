#include "baseactivity.h"

#include "../launcherapp.h"

using namespace ilixi;

BaseActivity::BaseActivity(const std::string &name, ilixi::Widget *parent): ContainerWidget(parent)
{
    _name = name;
    setVisible(false);
}

std::string BaseActivity::name() const
{
    return _name;
}

// Static
ilixi::Size BaseActivity::availableArea()
{
    return Size(LauncherApp::instance().width(),
                LauncherApp::instance().height() - LauncherApp::instance().stylist()->defaultParameter(StyleHint::ToolBarHeight)*2);
}

// Static
void BaseActivity::setWidgetFont(TextBase *widget, int increaseSizeValue, Font::Style style)
{
    Font *font = new Font(*widget->font());
    font->setSize(font->size() + increaseSizeValue);
    font->setStyle(style);
    widget->setFont(font);
}

