#include "LightStripWidget.h"
#include <QPainter>
#include <cmath>
#include "../mainwindow.h"

LightStripWidget *LightStripWidget::singleton = NULL;

LightStripWidget::LightStripWidget(QWidget *parent) : QWidget(parent),
    currentIndex(0),
    interval(1000),
    isFading(false),
    pulseDirection(true),
    isOn(true),
    currentEffect(NoEffect),
    currentOpacity(1.0),
    targetOpacity(1.0),
    isDimming(false),
    isBrightening(false)
{
    if (!singleton) {
        singleton = this;
    }

    setAttribute(Qt::WA_StyledBackground);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &LightStripWidget::updateAnimation);
    timer->setInterval(interval);

    setMinimumSize(200, 50);
    setSolidColor("red");
}

LightStripWidget::~LightStripWidget() {
    timer->stop();
    delete timer;
}

LightStripWidget *LightStripWidget::self()
{
    if (!singleton) {
        singleton = new LightStripWidget(MainWindow::self());
    }
    return singleton;
}

void LightStripWidget::setSolidColor(const QColor &color) {
    currentColor = color;
    currentEffect = SolidColor;
    update();
}

void LightStripWidget::dimLights() {
    if (currentEffect == SolidColor && currentOpacity > 0.125) {
        targetOpacity = qMax(0.125, currentOpacity / 2);
        isDimming = true;
        isBrightening = false;
        timer->start(100);  // Adjust the interval for smoother transitions
    }
}

void LightStripWidget::brightenLights() {
    if (currentEffect == SolidColor && currentOpacity < 1.0) {
        targetOpacity = qMin(1.0, currentOpacity * 2);
        isBrightening = true;
        isDimming = false;
        timer->start(100);  // Adjust the interval for smoother transitions
    }
}


void LightStripWidget::startFadeOnAndOff(const QList<QColor> &colors, int interval) {
    this->colors = colors;
    currentEffect = FadeOnAndOff;
    timer->start(interval);
}

void LightStripWidget::startFlashOnAndOff(const QList<QColor> &colors, int interval) {
    this->colors = colors;
    currentEffect = FlashOnAndOff;
    timer->start(interval);
}

void LightStripWidget::startRotateWithFade(const QList<QColor> &colors, int interval) {
    this->colors = colors;
    currentEffect = RotateWithFade;
    timer->start(interval);
}

void LightStripWidget::startRotateWithoutFade(const QList<QColor> &colors, int interval) {
    this->colors = colors;
    currentEffect = RotateWithoutFade;
    timer->start(interval);
}

void LightStripWidget::startPulse(const QList<QColor> &pulseColors, const QColor &backgroundColor, int interval, bool direction) {
    this->colors = pulseColors;
    this->backgroundColor = backgroundColor;
    pulseDirection = direction;
    currentEffect = Pulse;
    timer->start(interval);
}

void LightStripWidget::updateAnimation() {
    if (isDimming || isBrightening) {
        double step = 0.05;  // Control the speed of fading
        if (isDimming) {
            currentOpacity = qMax(targetOpacity, currentOpacity - step);
            if (currentOpacity <= targetOpacity) {
                isDimming = false;
                timer->stop();
            }
        } else if (isBrightening) {
            currentOpacity = qMin(targetOpacity, currentOpacity + step);
            if (currentOpacity >= targetOpacity) {
                isBrightening = false;
                timer->stop();
            }
        }
        update();
        return;
    }

    switch (currentEffect) {
    case FadeOnAndOff:
        if (isFading) {
            currentColor.setAlpha(currentColor.alpha() - 5);  // Adjust fade step size as needed
            if (currentColor.alpha() <= 0) {
                currentIndex = (currentIndex + 1) % colors.size();
                currentColor = colors[currentIndex];
                isFading = false;
            }
        } else {
            if (currentColor.alpha() < 255) {
                currentColor.setAlpha(currentColor.alpha() + 5);  // Adjust fade step size as needed
            } else {
                isFading = true;
            }
        }
        break;
    case FlashOnAndOff:
        if (isFading) {
            currentColor = Qt::transparent;
            isFading = false;
        } else {
            currentIndex = (currentIndex + 1) % colors.size();
            currentColor = colors[currentIndex];
            isFading = true;
        }
        break;
    case RotateWithFade:
        currentIndex = (currentIndex + 1) % colors.size();
        currentColor = colors[currentIndex];
        break;
    case RotateWithoutFade:
        currentIndex = (currentIndex + 1) % colors.size();
        currentColor = colors[currentIndex];
        break;
    case Pulse:
        currentIndex = (currentIndex + (pulseDirection ? 1 : -1)) % colors.size();
        if (currentIndex < 0) {
            currentIndex += colors.size();  // Correct for negative index
        }
        break;
    default:
        break;
    }
    update();
}


void LightStripWidget::togglePower(bool power) {
    isOn = power;
    if (!isOn) {
        colors.clear();
        currentColor = Qt::black;
    }
    update();
}

void LightStripWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    int numSections = colors.size();
    int sectionWidth = width() / numSections;

    if (isOn) {
        switch (currentEffect) {
        case SolidColor: {
            QColor displayColor = currentColor;
            displayColor.setAlphaF(currentOpacity); // Use currentOpacity for alpha value
            painter.fillRect(rect(), displayColor);
        }
            break;
        case FadeOnAndOff:
        case FlashOnAndOff:
            painter.fillRect(rect(), currentColor);
            break;
        case RotateWithFade:
        case RotateWithoutFade:
            for (int i = 0; i < numSections; ++i) {
                QColor color = colors[(currentIndex + i) % numSections];
                painter.fillRect(i * sectionWidth, 0, sectionWidth, height(), color);
            }
            break;
        case Pulse:
            for (int i = 0; i < numSections; ++i) {
                QColor color = (i == currentIndex) ? colors[currentIndex % colors.size()] : backgroundColor;
                painter.fillRect(i * sectionWidth, 0, sectionWidth, height(), color);
            }
            break;
        default:
            painter.fillRect(rect(), Qt::black);
            break;
        }
    } else {
        painter.fillRect(rect(), Qt::black);
    }
}

