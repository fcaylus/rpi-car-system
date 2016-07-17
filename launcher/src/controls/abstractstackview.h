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

#ifndef ABSTRACTSTACKVIEW_H
#define ABSTRACTSTACKVIEW_H

#include <QQuickItem>
#include <QQmlComponent>
#include <QStack>

class AbstractStackView : public QQuickItem
{
        Q_OBJECT

        Q_PROPERTY(QQuickItem* currentItem READ currentItem NOTIFY currentItemChanged)
        // The number of items pushed into the stack
        Q_PROPERTY(int depth READ depth NOTIFY depthChanged)

        Q_PROPERTY(bool animate READ animate WRITE setAnimate NOTIFY animateChanged)

    public:
        AbstractStackView(QQuickItem* parent = nullptr);

        QQuickItem* currentItem() const;
        int depth() const;

        bool animate() const;

    signals:

        void currentItemChanged();
        void depthChanged();

        // This signal is send before the deletion of the previous item
        // If animate is set to true, finishPush must be called when the animation complete
        void itemPushed();
        // Same as itemPushed()
        void itemPoped();

        void animateChanged();

    public slots:

        void pushItem(QQuickItem* item, bool deleteOnPop = false);
        // If an item is pushed from a component, it will be deleted on pop
        void push(QQmlComponent* component);
        void pop();

        void finishPush();
        void finishPop();

        void setAnimate(bool anim);

    private:

        QStack<QPair<QQuickItem*, bool>> _stack;

        bool _anim = false;
        QPair<QQuickItem*, bool> _pair;
};

#endif // ABSTRACTSTACKVIEW_H
