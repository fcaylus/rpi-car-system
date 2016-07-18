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

#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QQuickItem>

class ProgressBar: public QQuickItem
{
        Q_OBJECT

        Q_PROPERTY(qreal minimumValue READ minimumValue WRITE setMinimumValue NOTIFY minimumValueChanged)
        Q_PROPERTY(qreal maximumValue READ maximumValue WRITE setMaximumValue NOTIFY maximumValueChanged)
        Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged)
        // Return a percentage
        Q_PROPERTY(qreal position READ position NOTIFY positionChanged)

        Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

        Q_PROPERTY(QQuickItem* background READ background WRITE setBackground NOTIFY backgroundChanged)
        Q_PROPERTY(QQuickItem* indicator READ indicator WRITE setIndicator NOTIFY indicatorChanged)

    public:
        ProgressBar(QQuickItem* parent = nullptr);

        qreal minimumValue() const;
        qreal maximumValue() const;
        qreal value() const;
        qreal position() const;

        Qt::Orientation orientation() const;

        QQuickItem* background() const;
        QQuickItem* indicator() const;

    signals:

        void minimumValueChanged();
        void maximumValueChanged();
        void valueChanged();
        void positionChanged();

        void orientationChanged();

        void backgroundChanged();
        void indicatorChanged();

    public slots:

        void setMinimumValue(qreal min);
        void setMaximumValue(qreal max);
        void setValue(qreal val);

        void setOrientation(Qt::Orientation orientation);

        void setBackground(QQuickItem* background);
        void setIndicator(QQuickItem* indicator);

    protected:

        void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    private:

        qreal _min = 0.0;
        qreal _max = 1.0;
        qreal _value = 0.0;

        Qt::Orientation _orientation = Qt::Horizontal;

        QQuickItem *_background = nullptr;
        QQuickItem *_indicator = nullptr;

        void resizeBackground();
};

#endif // PROGRESSBAR_H
