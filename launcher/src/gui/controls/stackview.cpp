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

#include "stackview.h"

#include <QQmlComponent>

StackView::StackView(QQuickItem *parent): QQuickItem(parent)
{

}

//
// Getters
//

QQuickItem* StackView::currentItem() const
{
    if(_stack.isEmpty())
        return nullptr;
    return _stack.top().first;
}

int StackView::depth() const
{
    return _stack.size();
}

bool StackView::animate() const
{
    return _anim;
}

//
// Public slots
//

void StackView::setAnimate(bool anim)
{
    if(_anim != anim)
    {
        _anim = anim;
        emit animateChanged();
    }
}

void StackView::pushItem(QQuickItem *item, bool deleteOnPop)
{
    if(item)
    {
        if(!item->parentItem())
            item->setParentItem(this);

        if(!_anim)
            item->setPosition(QPointF(0,0));

        item->setSize(QSizeF(width(), height()));
        _stack.push(QPair<QQuickItem*, bool>(item, deleteOnPop));

        emit currentItemChanged();
        emit depthChanged();
        emit itemPushed();

        if(!_anim)
            finishPush();
    }
}

void StackView::push(QQmlComponent *component)
{
    if(component)
    {
        QObject* obj = component->create();
        QQuickItem *item = qobject_cast<QQuickItem*>(obj);
        if(item)
        {
            item->setParentItem(this);
            pushItem(item, true);
        }
    }
}

// Remove parent of the previous item
void StackView::finishPush()
{
    if(_stack.size() > 1)
        _stack.at(_stack.size() - 2).first->setParentItem(nullptr);
}

void StackView::pop()
{
    // Don't pop the first item
    if(_stack.size() < 2)
        return;

    _pair = _stack.pop();

    // Update the last item
    if(!_stack.top().first->parentItem())
        _stack.top().first->setParentItem(this);
    _stack.top().first->setSize(QSizeF(width(), height()));

    emit currentItemChanged();
    emit depthChanged();
    emit itemPoped();

    if(!_anim)
        finishPop();
}

// Handle the poped item
void StackView::finishPop()
{
    if(!_pair.first)
        return;

    if(_pair.second && _pair.first->parentItem() == this)
        _pair.first->deleteLater();
    else
        _pair.first->setParentItem(nullptr);
}
