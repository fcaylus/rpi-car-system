#include "launchertoolbar.h"

using namespace ilixi;

LauncherToolBar::LauncherToolBar(Widget *parent): ToolBar(parent)
{

}

// Size toolbar is 2x higher
Size LauncherToolBar::preferredSize() const
{
    const Size s = _layout->preferredSize();
    return Size(s.width() + _margin.hSum(), stylist()->defaultParameter(StyleHint::ToolBarHeight)*2);
}
