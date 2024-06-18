#include "lightstripwidget.h"
#include <QPainter>
#include <cmath>
#include "../mainwindow.h"
#include "QJsonObject"
#include "QtCore/qjsonarray.h"
#include "QJsonDocument"
#include "QFile"

#define DIM 0.5

LightStripWidget *LightStripWidget::singleton = NULL;

LightStripWidget::LightStripWidget(QWidget *parent) : QWidget(parent),
    currentIndex(0),
    cycleTime(1000),
    currentTime(0),
    isFading(false),
    pulseDirection(true),
    fadeBetweenColors(true),
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

    setFixedSize(250, 30);
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
    case OffAndOn:
        return "OffAndOn";
    case Rotate:
        return "Rotate";
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
    } else if (text == "OffAndOn") {
        return OffAndOn;
    } else if (text == "Rotate") {
        return Rotate;
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
    jObj["fadeBetweenColors"] = fadeBetweenColors;

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
    fadeBetweenColors = jObj.value("fadeBetweenColors").toBool(fadeBetweenColors);

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
    case OffAndOn:
        startOffAndOn(colors, cycleTime, fadeBetweenColors);
        break;
    case Rotate:
        startRotate(colors, cycleTime, fadeBetweenColors);
        break;
    case Pulse:
        startPulse(colors, backgroundColor, cycleTime, pulseDirection);
        break;
    }
}

void LightStripWidget::startTestEffect(Effect effect)
{
    currentColor = QColorConstants::Svg::purple;

    colors = {QColor("red"), QColor("white"), QColor("blue")};
    cycleTime = 1000;
    fadeBetweenColors = true;

    backgroundColor = Qt::black;
    pulseDirection = true;

    isDim = false;

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
    qDebug() << "brighting";
    if (currentEffect == SolidColor && !isDimming && !isBrightening && isDim) {
        isBrightening = true;

        cycleTime = 1000;
        currentTime = 0;
        timer->start();
        qDebug() << "brighting";
    }
}


void LightStripWidget::startOffAndOn(const QList<QColor> &colors, int interval, bool fade) {
    if (colors.size() < 1) {
        return;
    }

    this->colors = colors;
    currentEffect = OffAndOn;
    fadeBetweenColors = fade;
    currentIndex = 0;
    isFading = true; // start opaque

    cycleTime = interval;
    currentTime = 0;
    timer->start();
}

// fade between colors
void LightStripWidget::startRotate(const QList<QColor> &colors, int interval, bool fade) {
    if (colors.size() < 2) { // need at least 2
        return;
    }

    this->colors = colors;
    currentEffect = Rotate;
    fadeBetweenColors = fade;
    currentIndex = 0;
    isFading = true;

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
    case OffAndOn: {
        if (fadeBetweenColors) {
            currentColor = colors.at(currentIndex);
            float a = linearlyInterpolate(isFading, !isFading, progress);
            currentColor.setAlphaF(a);
        } else {
            currentColor = isFading ? colors.at(currentIndex) : Qt::transparent;
        }
    }
        break;
    case Rotate: {
        if (fadeBetweenColors) {
            QColor indexedColor = colors.at(currentIndex);
            QColor nextColor = colors.at((currentIndex + 1) % colors.size());
            currentColor = blendColors(indexedColor, nextColor, progress);
        } else {
            currentColor = colors.at(currentIndex);
        }
    }
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

        if (currentEffect == OffAndOn) {
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
        case OffAndOn:
        case Rotate:
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











