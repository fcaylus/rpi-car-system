#include "baseactivity.h"

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

