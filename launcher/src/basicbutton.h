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

#ifndef BASICBUTTON_H
#define BASICBUTTON_H

#include <QQuickItem>

// It's a lightweight alternative to QtQuickControl Button's
class BasicButton : public QQuickItem
{
        Q_OBJECT

        Q_PROPERTY(bool pressed READ isPressed WRITE setPressed NOTIFY pressedChanged)
        Q_PROPERTY(bool checked READ isChecked WRITE setChecked NOTIFY checkedChanged)
        Q_PROPERTY(bool checkable READ isCheckable WRITE setCheckable NOTIFY checkableChanged)
        Q_PROPERTY(bool autoRepeat READ autoRepeat WRITE setAutoRepeat NOTIFY autoRepeatChanged)
        Q_PROPERTY(int repeatInterval READ repeatInterval WRITE setRepeatInterval NOTIFY repeatIntervalChanged)

        Q_PROPERTY(bool enablePressAndHold READ isPressAndHoldEnabled WRITE setEnablePressAndHold NOTIFY enablePressAndHoldChanged)

        Q_PROPERTY(bool hasSecondState READ hasSecondState WRITE setHasSecondState NOTIFY hasSecondStateChanged)
        // Set to true if the button is in the second state
        Q_PROPERTY(bool secondStateEnabled READ isSecondStateEnabled WRITE setSecondStateEnabled NOTIFY secondStateEnabledChanged)

        // The order of the state (starting at 0) is:
        //
        // - 0: unchecked
        // - 1: checked if checkable set to true
        // - 2: second state if secondState set to true (this state will be checked if checkable set to true)
        Q_PROPERTY(int currentState READ currentState NOTIFY currentStateChanged)

        Q_PROPERTY(QQuickItem* background READ background WRITE setBackground NOTIFY backgroundChanged)
        Q_PROPERTY(QQuickItem* label READ label WRITE setLabel NOTIFY labelChanged)

    public:
        BasicButton(QQuickItem* parent = nullptr);

        bool isPressed() const;
        bool isChecked() const;
        bool isCheckable() const;
        bool autoRepeat() const;
        int repeatInterval() const;

        bool isPressAndHoldEnabled() const;

        bool hasSecondState() const;
        bool isSecondStateEnabled() const;

        int currentState() const;

        QQuickItem* background() const;
        QQuickItem* label() const;

    signals:

        void pressedChanged();
        void pressed();
        void released();
        void clicked();
        void canceled();
        void pressAndHold();
        void enablePressAndHoldChanged();

        void checkedChanged();
        void checkableChanged();
        void autoRepeatChanged();
        void repeatIntervalChanged();

        void hasSecondStateChanged();
        void secondStateEnabledChanged();
        void currentStateChanged();

        void backgroundChanged();
        void labelChanged();

    public slots:

        void setPressed(bool pressed);
        void setChecked(bool checked);
        void setCheckable(bool checkable);
        void setAutoRepeat(bool repeat);
        void setRepeatInterval(int interval);

        void setEnablePressAndHold(bool enable);

        void setHasSecondState(bool hasSecondState);
        void setSecondStateEnabled(bool enabled);

        void setBackground(QQuickItem* background);
        void setLabel(QQuickItem* label);

        // These functions don't work with the secondState mechanism
        void toggle();
        void click();

    protected:

        void focusOutEvent(QFocusEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void mouseMoveEvent(QMouseEvent *event) override;
        void mouseReleaseEvent(QMouseEvent *event) override;
        void mouseUngrabEvent() override;
        void timerEvent(QTimerEvent *event) override;

        void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry) override;

    private:

        bool _pressed = false;
        bool _checked = false;
        bool _checkable = false;
        bool _autoRepeat = false;
        int _repeatInterval = 100;

        bool _pressAndHoldEnabled = false;

        bool _hasSecond = false;
        bool _secondEnabled = false;

        QQuickItem *_background = nullptr;
        QQuickItem *_label = nullptr;

        void startPressAndHold();
        void stopPressAndHold();

        void startRepeatDelay();
        void startPressRepeat();
        void stopPressRepeat();

        void resizeBackground();
        void resizeLabel();

        void updateImplicitWidth();

        bool _wasHeld = false;
        int _holdTimer = 0;
        int _delayTimer = 0;
        int _repeatTimer = 0;

        QPointF _pressPoint;
};

#endif // BASICBUTTON_H
