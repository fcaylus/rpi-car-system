#ifndef CUSTOMICON_H
#define CUSTOMICON_H

#include <ui/Icon.h>
#include <functional>

class CustomIcon : public ilixi::Icon
{
    public:
        CustomIcon(const std::string& path, std::function<ilixi::Size(void)> sizeFunction, Widget* parent = 0);

        ilixi::Size preferredSize() const;

    private:

        std::function<ilixi::Size(void)> _sizeFunc = [](){ return ilixi::Size(-1,-1); };
};

#endif // CUSTOMICON_H
