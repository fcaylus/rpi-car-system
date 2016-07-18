/*
 * This file is part of RPI Car System.
 * Copyright (c) 2016 Fabien Caylus <tranqyll.dev@gmail.com>
 *
 * This file is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "button.h"

#include <QMouseEvent>
#include <QFocusEvent>
#include <QTimerEvent>
#include <QPointF>
#include <QRectF>

Button::Button(QQuickItem *parent): QQuickItem(parent)
{
    setAcceptedMouseButtons(Qt::LeftButton);
}

//
// Getters
//

bool Button::isPressed() const
{
    return _pressed;
}

bool Button::isChecked() const
{
    return _checked;
}

bool Button::isCheckable() const
{
    return _checkable;
}

bool Button::autoRepeat() const
{
    return _autoRepeat;
}

bool Button::isPressAndHoldEnabled() const
{
    return _pressAndHoldEnabled;
}

int Button::repeatInterval() const
{
    return _repeatInterval;
}

bool Button::hasSecondState() const
{
    return _hasSecond;
}

bool Button::isSecondStateEnabled() const
{
    return _secondEnabled;
}

int Button::currentState() const
{
    return _hasSecond && _secondEnabled ? 2 : (_checkable && _checked ? 1 : 0);
}

QQuickItem* Button::background() const
{
    return _background;
}

QQuickItem* Button::label() const
{
    return _label;
}

//
// Setters
//

void Button::setPressed(bool pressed)
{
    if(_pressed != pressed)
    {
        _pressed = pressed;
        emit pressedChanged();
    }
}

void Button::setChecked(bool checked)
{
    if(checked && !_checkable)
        setCheckable(true);

    if(checked != _checked)
    {
        _checked = checked;
        emit checkedChanged();
        emit currentStateChanged();
    }
}

void Button::setCheckable(bool checkable)
{
    if(_checkable != checkable)
    {
        _checkable = checkable;
        emit checkableChanged();
    }
}

void Button::setAutoRepeat(bool repeat)
{
    if(_autoRepeat != repeat)
    {
        stopPressRepeat();
        _autoRepeat = repeat;
        emit autoRepeatChanged();
    }
}

void Button::setRepeatInterval(int repeat)
{
    if(_repeatInterval != repeat)
    {
        if(_repeatInterval <= 0)
            _repeatInterval = 100;

        _repeatInterval = repeat;
        emit repeatIntervalChanged();
    }
}

void Button::setEnablePressAndHold(bool enable)
{
    if(_pressAndHoldEnabled != enable)
    {
        _pressAndHoldEnabled = enable;
        if(!_pressAndHoldEnabled)
            stopPressAndHold();
        emit enablePressAndHoldChanged();
    }
}

void Button::setHasSecondState(bool hasSecondState)
{
    if(hasSecondState != _hasSecond)
    {
        // Restore the default state
        if(_hasSecond && _secondEnabled)
            setSecondStateEnabled(false);

        _hasSecond = hasSecondState;
        emit hasSecondStateChanged();
    }
}

void Button::setSecondStateEnabled(bool enabled)
{
    if(_secondEnabled != enabled)
    {
        if(_secondEnabled)
            setChecked(false);

        _secondEnabled = enabled;
        emit secondStateEnabledChanged();
        emit currentStateChanged();
    }
}

void Button::setBackground(QQuickItem *background)
{
    if(background != _background)
    {
        delete _background;
        _background = background;
        if(background)
        {
            background->setParentItem(this);
            if(qFuzzyIsNull(background->z()))
                background->setZ(-1);
            if(isComponentComplete())
                resizeBackground();

            updateImplicitWidth();
        }
        emit backgroundChanged();
    }
}

void Button::setLabel(QQuickItem *label)
{
    if(_label != label)
    {
        delete _label;
        _label = label;
        if(label)
        {
            if(!label->parentItem())
                label->setParentItem(this);
            if(isComponentComplete())
                resizeLabel();

            updateImplicitWidth();
        }
        emit labelChanged();
    }
}

void Button::toggle()
{
    setChecked(!_checked);
}

// Simulate a click
void Button::click()
{
    setPressed(true);
    emit pressed();

    setPressed(false);
    emit released();
    emit clicked();
}

//
// Re-implemented
//

void Button::focusOutEvent(QFocusEvent *event)
{
    QQuickItem::focusOutEvent(event);
    if(_pressed)
    {
        setPressed(false);
        emit canceled();
    }

}

void Button::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    event->accept();
    setPressed(true);
    _pressPoint = event->pos();
    emit pressed();

    if(Qt::LeftButton == (event->buttons() & Qt::LeftButton))
    {
        if(_autoRepeat)
            startRepeatDelay();
        else
            startPressAndHold();
    }
    else
        stopPressAndHold();
}

void Button::mouseMoveEvent(QMouseEvent *event)
{
    QQuickItem::mouseMoveEvent(event);
    event->accept();
    setPressed(contains(event->pos()));

    if(!contains(event->pos()))
    {
        if(_autoRepeat)
            stopPressRepeat();
        else if(_holdTimer > 0)
            stopPressAndHold();
    }
}

void Button::mouseReleaseEvent(QMouseEvent *event)
{
    QQuickItem::mouseReleaseEvent(event);
    event->accept();
    const bool wasPressed = _pressed;
    setPressed(false);

    if(wasPressed)
    {
        emit released();
        if(!_wasHeld)
            emit clicked();
    }
    else
        emit canceled();

    if(contains(event->pos()))
    {
        // Handle all checked states
        if(!_hasSecond)
        {
            // Starndard behavior
            if(_checkable)
                setChecked(!_checked);
        }
        else
        {
            // Move to the next state
            if(!_secondEnabled)
            {
                if(_checkable && !_checked)
                    setChecked(true);
                else
                    setSecondStateEnabled(true);
            }
            else
                setSecondStateEnabled(false);
        }
    }


    if(_autoRepeat)
        stopPressRepeat();
    else
        stopPressAndHold();
}

void Button::mouseUngrabEvent()
{
    QQuickItem::mouseUngrabEvent();
    if(_pressed)
    {
        setPressed(false);
        stopPressRepeat();
        stopPressAndHold();
        emit canceled();
    }
}

void Button::timerEvent(QTimerEvent *event)
{
    QQuickItem::timerEvent(event);
    if(event->timerId() == _holdTimer)
    {
        stopPressAndHold();
        _wasHeld = true;
        emit pressAndHold();
    }
    else if(event->timerId() == _delayTimer)
    {
        startPressRepeat();
    }
    else if(event->timerId() == _repeatTimer)
    {
        emit released();
        emit clicked();
        emit pressed();
    }
}

void Button::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    updateImplicitWidth();
    resizeBackground();
    resizeLabel();
}

//
// Resize
//

void Button::resizeBackground()
{
    if(_background)
    {
        if(qFuzzyIsNull(_background->x()))
            _background->setWidth(width());

        if(qFuzzyIsNull(_background->y()))
            _background->setHeight(height());
    }
}

void Button::resizeLabel()
{
    if(_label)
    {
        _label->setPosition(QPointF(0,0));
        _label->setSize(QSizeF(width(), height()));
    }
}

void Button::updateImplicitWidth()
{
    setImplicitWidth(qMax(_background ? _background->implicitWidth() : 0,
                          _label ? _label->implicitWidth() : 0));

    setImplicitHeight(qMax(_background ? _background->implicitHeight() : 0,
                           _label ? _label->implicitHeight() : 0));
}

//
// Press and hold
//

void Button::startPressAndHold()
{
    _wasHeld = false;
    stopPressAndHold();
    if(isPressAndHoldEnabled())
        _holdTimer = startTimer(800);
}

void Button::stopPressAndHold()
{
    if(_holdTimer > 0)
    {
        killTimer(_holdTimer);
        _holdTimer = 0;
    }
}

//
// Auto repeat
//

void Button::startRepeatDelay()
{
    stopPressRepeat();
    _delayTimer = startTimer(300);
}

void Button::startPressRepeat()
{
    stopPressRepeat();
    _repeatTimer = startTimer(_repeatInterval);
}

void Button::stopPressRepeat()
{
    if(_delayTimer > 0)
    {
        killTimer(_delayTimer);
        _delayTimer = 0;
    }
    if(_repeatTimer > 0)
    {
        killTimer(_repeatTimer);
        _repeatTimer = 0;
    }
}
