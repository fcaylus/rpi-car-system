#include "customicon.h"

CustomIcon::CustomIcon(const std::string &path, std::function<ilixi::Size (void)> sizeFunction, Widget *parent) : ilixi::Icon(path, parent)
{
    _sizeFunc = sizeFunction;
}

ilixi::Size CustomIcon::preferredSize() const
{
    return _sizeFunc();
}
