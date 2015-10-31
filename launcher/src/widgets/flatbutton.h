#ifndef FLATBUTTON_H
#define FLATBUTTON_H

#include <ui/ToolButton.h>
#include <ui/Icon.h>
#include <lib/Timer.h>
#include <string>
#include <vector>
#include <functional>

class FlatIcon;

class FlatButton : public ilixi::ToolButton
{
    public:
        // Will assume it's a launcher icon
        FlatButton(const std::string& iconName, int width = 0, bool enableLongPress = false, ilixi::Widget* parent = 0);
        ~FlatButton();

        // Add a state (for example, play <-> pause)
        // When the button is checkable, there is 3 states and the second
        // is always the checked version
        void addAdditionnalState(const std::string& iconName);

        int stateNumber();
        int currentState();

        void setCurrentState(int state);

        void setOnClickHandler(std::function<void(int)> handler);

        // Re-imp
        void setIcon(FlatIcon* icon);
        // Re-imp
        void setCheckable(bool checkable);
        // Re-imp
        ilixi::Size preferredSize() const;

    protected:
        virtual void compose(const ilixi::PaintEvent &event);

    private:

        bool _checkable;

        int _currentState;
        std::vector<FlatIcon *> _icons;

        bool _checked = false;

        ilixi::Timer _longPressTimer; // First timer, after a delay
        ilixi::Timer _longPressTimer2; // Second timer, fast click
        bool _enableLongPress = false;

        std::function<void(int)> _onClickHandler = [](int newState){ (void)newState; };
};

class FlatIcon: public ilixi::Icon
{
    public:
        // If width = 0, assume it's a launcher icon
        FlatIcon(const std::string& iconName, int width = 0, ilixi::Widget* parent = 0);

    protected:
        virtual void compose(const ilixi::PaintEvent &event);
};

#endif // FLATBUTTON_H
