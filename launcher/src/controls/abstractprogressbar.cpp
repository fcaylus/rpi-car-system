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

#include "abstractprogressbar.h"

AbstractProgressBar::AbstractProgressBar(QQuickItem *parent): QQuickItem(parent)
{
}

//
// Getters
//

qreal AbstractProgressBar::minimumValue() const
{
    return _min;
}

qreal AbstractProgressBar::maximumValue() const
{
    return _max;
}

qreal AbstractProgressBar::value() const
{
    return _value;
}

qreal AbstractProgressBar::position() const
{
    if(qFuzzyCompare(_min, _max))
        return 0.0;
    return (_value - _min) / (_max - _min);
}

Qt::Orientation AbstractProgressBar::orientation() const
{
    return _orientation;
}

QQuickItem* AbstractProgressBar::background() const
{
    return _background;
}

QQuickItem* AbstractProgressBar::indicator() const
{
    return _indicator;
}

//
// Setters
//

void AbstractProgressBar::setMinimumValue(qreal min)
{
    if(!qFuzzyCompare(min, _min) && min < _max)
    {
        _min = min;
        emit minimumValueChanged();
        emit positionChanged();
        if(_value < _min)
            setValue(_min);
    }
}

void AbstractProgressBar::setMaximumValue(qreal max)
{
    if(!qFuzzyCompare(max, _max) && max > _min)
    {
        _max = max;
        emit maximumValueChanged();
        emit positionChanged();
        if(_value > _max)
            setValue(_max);
    }
}

void AbstractProgressBar::setValue(qreal val)
{
    val = qBound(_min, val, _max);
    if(!qFuzzyCompare(val, _value))
    {
        _value = val;
        emit valueChanged();
        emit positionChanged();
    }
}

void AbstractProgressBar::setOrientation(Qt::Orientation orientation)
{
    if(_orientation != orientation)
    {
        _orientation = orientation;
        emit orientationChanged();
    }
}

void AbstractProgressBar::setBackground(QQuickItem *background)
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
        }
        emit backgroundChanged();
    }
}

void AbstractProgressBar::setIndicator(QQuickItem *indicator)
{
    if(_indicator != indicator)
    {
        delete _indicator;
        _indicator = indicator;
        if(indicator && !indicator->parentItem())
            indicator->setParentItem(this);
        emit indicatorChanged();
    }
}

//
// Re-implemented
//

void AbstractProgressBar::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    QQuickItem::geometryChanged(newGeometry, oldGeometry);
    resizeBackground();
}

//
// Resize
//

void AbstractProgressBar::resizeBackground()
{
    if(_background)
    {
        if(qFuzzyIsNull(_background->x()))
            _background->setWidth(width());

        if(qFuzzyIsNull(_background->y()))
            _background->setHeight(height());
    }
}
