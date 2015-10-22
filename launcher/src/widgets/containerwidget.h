#ifndef CONTAINERWIDGET_H
#define CONTAINERWIDGET_H

#include <ui/Frame.h>

// A simple widget to store other widgets without frame
class ContainerWidget : public ilixi::Frame
{
    public:
        ContainerWidget(Widget* parent = 0);
        ~ContainerWidget();

        /*bool removeAllChildren(bool destroy = true);
        bool hasChildren();*/
};

#endif // CONTAINERWIDGET_H
