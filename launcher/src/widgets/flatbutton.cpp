#include "flatbutton.h"
#include "../launcherapp.h"

#include "easylogging++.h"

#include <graphics/Painter.h>

using namespace ilixi;

FlatButton::FlatButton(const std::string &iconName, int width, bool enableLongPress, Widget *parent): ToolButton("", parent)
{
    setDrawFrame(false);

    _enableLongPress = enableLongPress;

    FlatIcon *icon = new FlatIcon(iconName, width, this);
    _icons.push_back(icon);
    setIcon(icon);
    _currentState = 0;

    setToolButtonStyle(IconOnly);

    sigClicked.connect([this](){
        if(stateNumber() > 1)
            setCurrentState(_currentState + 1);
        _onClickHandler(currentState());
    });

    if(_enableLongPress)
    {
        _longPressTimer2.sigExec.connect([this]() {
            if(!_checked)
                _longPressTimer2.stop();
            else
            {
                _onClickHandler(currentState());
            }
        });
        _longPressTimer.sigExec.connect([this]() {
            if(_checked)
                _longPressTimer2.start(70, 100);
        });
    }

    // Animate the button when pressed
    sigPressed.connect([this](){
        _checked = true;
        translate(0, 2);

        // Handle long press
        if(_enableLongPress)
        {
            _longPressTimer.stop();
            _longPressTimer2.stop();

            _longPressTimer.start(500, 1);
        }
    });
    sigReleased.connect([this](){
        _checked = false;
        translate(0, -2);
    });
}

void FlatButton::addAdditionnalState(const std::string &iconName)
{
    _icons.push_back(new FlatIcon(iconName, 0, this));
}

int FlatButton::stateNumber()
{
    return _icons.size() + (_checkable ? 1 : 0);
}

int FlatButton::currentState()
{
    return _currentState;
}

void FlatButton::setCurrentState(int state)
{
    if(state == _currentState)
        return;

    _currentState = state % stateNumber();

    if(checkable())
        setChecked(_currentState >= 1);

    // Check if the widget is checkable
    if(_checkable)
    {
        if(_currentState == 0)
            setIcon(_icons[_currentState]);
        else if(_currentState > 1)
            setIcon(_icons[_currentState - 1]);
    }
    else
    {
        setIcon(_icons[_currentState]);
    }
}

void FlatButton::setOnClickHandler(std::function<void (int)> handler)
{
    _onClickHandler = handler;
}

// Public re-implemented
void FlatButton::setIcon(FlatIcon *icon)
{
    removeChild(_icon, false);
    _icon = icon;
    addChild(_icon);
}

// Public re-implemented
void FlatButton::setCheckable(bool checkable)
{
    _checkable = checkable;
    ToolButton::setCheckable(checkable);
}

// Public re-implemented
Size FlatButton::preferredSize() const
{
    // Calculate image size
    Size s;
    if (_icon->size().isValid())
        s = _icon->size();
    else
        s = _icon->preferredSize();

    s.setHeight(s.height() + 4); // 2px for button down movement
    return s;
}


// Protected re-implemented
void FlatButton::compose(const PaintEvent &/*event*/)
{
}

//------------------------------------------
//--- FlatIcon -----------------------------
//------------------------------------------

FlatIcon::FlatIcon(const std::string &iconName, int width, Widget* parent): Icon(parent)
{
    setImage(LauncherApp::iconPathForName(iconName));

    // Launcher icon
    if(width == 0)
        setSize(Size(LauncherApp::instance().stylist()->defaultParameter(StyleHint::ToolBarButtonHeight)*2,
                     LauncherApp::instance().stylist()->defaultParameter(StyleHint::ToolBarButtonHeight)*2));
    else
        setSize(Size(width, width));

    setColorize(false);
}

// Protected re-implemented
void FlatIcon::compose(const PaintEvent &event)
{
    Painter p(this);
    p.begin(event);

    bool colored = false;

    // Check parent state
    FlatButton* parentBtn = nullptr;
    if((parentBtn = static_cast<FlatButton *>(parent())) != nullptr)
    {
        if(parentBtn->checkable())
        {
            if(parentBtn->checked())
            {
                // Button checked & checkable
                p.setBrush(Color(51, 204, 255));
                colored = true;
            }
        }
        if(parentBtn->pressed())
        {
            // Button pressed & checkable
            p.setBrush(Color(26, 102, 128));
            colored = true;
        }
    }

    if(colored)
        p.stretchImage(image(), Rectangle(0, 0, width(), height()), (DFBSurfaceBlittingFlags) (DSBLIT_COLORIZE | DSBLIT_BLEND_ALPHACHANNEL));
    else
        p.stretchImage(image(), Rectangle(0, 0, width(), height()));

    p.end();
}



