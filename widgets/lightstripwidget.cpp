#include "lightstripwidget.h"
#include <QPainter>
#include <cmath>
#include "../mainwindow.h"
#include "QJsonObject"
#include "QtCore/qjsonarray.h"

#define DIM 0.5

LightStripWidget *LightStripWidget::singleton = NULL;

LightStripWidget::LightStripWidget(QWidget *parent) : QWidget(parent),
    currentIndex(0),
    cycleTime(1000),
    currentTime(0),
    isFading(false),
    pulseDirection(true),
    isOn(true),
    currentEffect(SolidColor),
    isDimming(false),
    isBrightening(false)
{
    if (!singleton) {
        singleton = this;
    }

    timer = new QTimer(this);
    timer->setSingleShot(false);
    timer->setInterval(10);

    connect(timer, &QTimer::timeout, this, &LightStripWidget::updateAnimation);

    setMinimumSize(200, 50);
    setSolidColor(Qt::white);
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

QString LightStripWidget::effectToString(Effect effect)
{
    switch (effect) {
    case SolidColor:
        return "SolidColor";
    case FadeOffAndOn:
        return "FadeOffAndOn";
    case FlashOffAndOn:
        return "FlashOffAndOn";
    case RotateWithFade:
        return "RotateWithFade";
    case RotateWithoutFade:
        return "RotateWithoutFade";
    case Pulse:
        return "Pulse";
    default:
        return "Unknown";
    }
}

LightStripWidget::Effect LightStripWidget::effectFromString(QString text)
{
    if (text == "SolidColor") {
        return SolidColor;
    } else if (text == "FadeOffAndOn") {
        return FadeOffAndOn;
    } else if (text == "FlashOffAndOn") {
        return FlashOffAndOn;
    } else if (text == "RotateWithFade") {
        return RotateWithFade;
    } else if (text == "RotateWithoutFade") {
        return RotateWithoutFade;
    } else if (text == "Pulse") {
        return Pulse;
    } else {
        return SolidColor;
    }
}

QStringList LightStripWidget::getEffectList()
{
    QStringList list;
    for (int e = Min; e <= Max; e++) {
        list.append(effectToString((Effect)e));
    }
    return list;
}

QJsonObject LightStripWidget::getJsonObject()
{
    QJsonObject jObj;

    jObj["currentEffect"] = effectToString(currentEffect);
    jObj["cycleTime"] = cycleTime;

    QJsonArray colorsArray;
    for (const QColor &color : colors) {
        colorsArray.append(color.name());
    }
    jObj["colors"] = colorsArray;

    jObj["backgroundColor"] = backgroundColor.name();
    jObj["pulseDirection"] = pulseDirection;

    jObj["isDim"] = isDim;

    return jObj;
}

void LightStripWidget::loadJsonObject(const QJsonObject &jObj)
{
    QString effectString = jObj.value("currentEffect").toString(effectToString(currentEffect));
    currentEffect = effectFromString(effectString);

    cycleTime = jObj.value("cycleTime").toInt(cycleTime);

    QJsonArray colorsArray = jObj.value("colors").toArray();
    colors.clear();
    for (const QJsonValue &value : colorsArray) {
        if (value.isString()) {
            QColor color(value.toString());
            colors.append(color);
        }
    }

    backgroundColor = QColor(jObj.value("backgroundColor").toString(backgroundColor.name()));
    pulseDirection = jObj.value("pulseDirection").toBool(pulseDirection);

    isDim = jObj.value("isDim").toBool(isDim);

    startEffect(currentEffect);
}

void LightStripWidget::startEffect(Effect effect)
{
    switch (effect) {
    case SolidColor:
        currentColor.setAlphaF(isDim ? DIM : 1.0);
        setSolidColor(currentColor);
        break;
    case FadeOffAndOn:
        startFadeOffAndOn(colors, cycleTime);
        break;
    case FlashOffAndOn:
        startFlashOffAndOn(colors, cycleTime);
        break;
    case RotateWithFade:
        startRotateWithFade(colors, cycleTime);
        break;
    case RotateWithoutFade:
        startRotateWithoutFade(colors, cycleTime);
        break;
    case Pulse:
        startPulse(colors, backgroundColor, cycleTime, pulseDirection);
        break;
    }
}

void LightStripWidget::startTestEffect(Effect effect)
{
    switch (effect) {
    case SolidColor:
        currentColor = QColorConstants::Svg::purple;
        break;
    case FadeOffAndOn:
    case FlashOffAndOn:
    case RotateWithFade:
    case RotateWithoutFade:
        colors = {QColor("red"), QColor("white"), QColor("blue")};
        cycleTime = 1000;
        break;
    case Pulse:
        startPulse(colors, backgroundColor, cycleTime, pulseDirection);
        break;
    }

    startEffect(effect);
}

void LightStripWidget::setSolidColor(const QColor &color) {
    currentColor = color;
    currentEffect = SolidColor;
    update();
}

// animate down the opacity of the solid color
void LightStripWidget::dimLights() {
    if (currentEffect == SolidColor && !isDimming && !isBrightening && !isDim) {
        isDimming = true;

        cycleTime = 1000;
        currentTime = 0;
        timer->start();
    }
}

// animate up the opacity of the solid color
void LightStripWidget::brightenLights() {
    if (currentEffect == SolidColor && !isDimming && !isBrightening && isDim) {
        isBrightening = true;

        cycleTime = 1000;
        currentTime = 0;
        timer->start();
    }
}


void LightStripWidget::startFadeOffAndOn(const QList<QColor> &colors, int interval) {
    if (colors.size() < 1) {
        return;
    }

    this->colors = colors;
    currentEffect = FadeOffAndOn;
    currentIndex = 0;
    isFading = true; // start opaque

    cycleTime = interval;
    currentTime = 0;
    timer->start();
}

void LightStripWidget::startFlashOffAndOn(const QList<QColor> &colors, int interval) {
    if (colors.size() < 1) {
        return;
    }

    this->colors = colors;
    currentEffect = FlashOffAndOn;
    currentIndex = 0;
    isFading = true;

    cycleTime = interval;
    currentTime = 0;
    timer->start();
}

// fade between colors
void LightStripWidget::startRotateWithFade(const QList<QColor> &colors, int interval) {
    if (colors.size() < 2) { // need at least 2
        return;
    }

    this->colors = colors;
    currentEffect = RotateWithFade;
    currentIndex = 0;

    cycleTime = interval;
    currentTime = 0;
    timer->start();
}

void LightStripWidget::startRotateWithoutFade(const QList<QColor> &colors, int interval) {
    if (colors.size() < 2) { // need at least 2
        return;
    }

    this->colors = colors;
    currentEffect = RotateWithoutFade;
    currentIndex = 0;

    cycleTime = interval;
    currentTime = 0;
    timer->start();
}

void LightStripWidget::startPulse(const QList<QColor> &pulseColors, const QColor &backgroundColor, int interval, bool direction) {
    if (colors.size() < 1) {
        return;
    }

    this->colors = pulseColors;
    this->backgroundColor = backgroundColor;
    pulseDirection = direction;
    currentEffect = Pulse;

    cycleTime = interval;
    currentTime = 0;
    timer->start();
}

void LightStripWidget::updateAnimation() {
    // interpolate some property over time

    currentTime += timer->interval();
    progress = qBound(0.0, (float)currentTime / cycleTime, 1.0);


    if (isDimming) {
        float a = linearlyInterpolate(1.0, DIM, progress);
        currentColor.setAlphaF(a);
        update();
        return;
    }
    if (isBrightening) {
        float a = linearlyInterpolate(DIM, 1.0, progress);
        currentColor.setAlphaF(a);
        update();
        return;
    }

    switch (currentEffect) {
    case FadeOffAndOn: {
        currentColor = colors.at(currentIndex);
        float a = linearlyInterpolate(isFading, !isFading, progress);
        currentColor.setAlphaF(a);
    }
        break;
    case FlashOffAndOn:
        currentColor = isFading ? colors.at(currentIndex) : Qt::transparent;
        break;
    case RotateWithFade: {
        QColor indexedColor = colors.at(currentIndex);
        QColor nextColor = colors.at((currentIndex + 1) % colors.size());
        currentColor = blendColors(indexedColor, nextColor, progress);
    }
        break;
    case RotateWithoutFade:
        currentColor = colors.at(currentIndex);
        break;
    case Pulse:
        currentColor = colors.at(currentIndex);
        break;
    default:
        break;
    }
    update();

    if (currentTime >= cycleTime) {
        if (isDimming) isDim = true;
        isDimming = false;
        isBrightening = false;

        if (currentEffect == FadeOffAndOn || currentEffect == FlashOffAndOn) {
            if (isFading) currentIndex = (currentIndex + 1) % colors.size(); // go to next color once faded
        } else {
            currentIndex = (currentIndex + 1) % colors.size(); // go to next color
        }

        isFading = !isFading; // reverse direction
        currentTime = 0;
    }
}


void LightStripWidget::togglePower(bool power) {
    isOn = power;
    update();
}

void LightStripWidget::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    if (isOn) {
        switch (currentEffect) {
        case SolidColor:
        case FadeOffAndOn:
        case FlashOffAndOn:
        case RotateWithFade:
        case RotateWithoutFade:
            painter.fillRect(rect(), currentColor);
            break;
        case Pulse: {
            painter.fillRect(rect(), backgroundColor);

            int x = linearlyInterpolate(pulseDirection ? 0.0 : width(), pulseDirection ? width() : 0.0, progress);
            int size = width() / 7;
            QRect pulseRect(x - size / 2, 0, size, height());
            painter.fillRect(pulseRect, currentColor);
        }
            break;
        }
    } else {
        painter.fillRect(rect(), Qt::transparent);
    }
}

LightStripWidget::Effect LightStripWidget::getCurrentEffect() const
{
    return currentEffect;
}



QColor LightStripWidget::blendColors(const QColor& startColor, const QColor& endColor, double progress) {
    int r = linearlyInterpolate(startColor.red(), endColor.red(), progress);
    int g = linearlyInterpolate(startColor.green(), endColor.green(), progress);
    int b = linearlyInterpolate(startColor.blue(), endColor.blue(), progress);

    return QColor(r, g, b);
}

double LightStripWidget::linearlyInterpolate(double startVal, double endVal, double progress)
{
    progress = qBound(0.0, progress, 1.0);
    return startVal * (1 - progress) + endVal * progress;
}











