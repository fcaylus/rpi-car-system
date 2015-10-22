#include "containerwidget.h"

using namespace ilixi;

ContainerWidget::ContainerWidget(Widget *parent): Frame(parent)
{
    setDrawFrame(false);
}

ContainerWidget::~ContainerWidget()
{

}

/*
bool ContainerWidget::removeAllChildren(bool destroy)
{
    if(!hasChildren())
        return true;

    bool returnValue = true;
    for(auto &child : _children)
    {
        if(!removeChild(child, destroy))
            returnValue = false;
    }

    return returnValue;
}

bool ContainerWidget::hasChildren()
{
    return !_children.empty();
}*/

