#ifndef CONTAINERWIDGET_H
#define CONTAINERWIDGET_H

#include "ilixiGUI.h"

// A simple widget to store other widgets without frame
class ContainerWidget : public ilixi::Frame
{
    public:
        ContainerWidget(Widget* parent = 0);
        ~ContainerWidget();
};

#endif // CONTAINERWIDGET_H
