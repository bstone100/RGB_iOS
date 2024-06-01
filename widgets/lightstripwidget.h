#ifndef LIGHTSTRIPWIDGET_H
#define LIGHTSTRIPWIDGET_H

#include <QWidget>
#include <QColor>
#include <QTimer>
#include <vector>

class LightStripWidget : public QWidget {
    Q_OBJECT

public:
    explicit LightStripWidget(QWidget *parent = nullptr);
    ~LightStripWidget();

    static LightStripWidget *self();

    enum Effect {
        NoEffect,
        SolidColor,
        FadeOnAndOff,
        FlashOnAndOff,
        RotateWithFade,
        RotateWithoutFade,
        Pulse
    };

    void setSolidColor(const QColor &color);
    void dimLights();
    void brightenLights();

    void startFadeOnAndOff(const QList<QColor> &colors, int interval);
    void startFlashOnAndOff(const QList<QColor> &colors, int interval);
    void startRotateWithFade(const QList<QColor> &colors, int interval);
    void startRotateWithoutFade(const QList<QColor> &colors, int interval);
    void startPulse(const QList<QColor> &pulseColors, const QColor &backgroundColor, int interval, bool direction);
    void paintEvent(QPaintEvent *event) override;

private:
    static LightStripWidget *singleton;

    QTimer *timer;
    QList<QColor> colors;
    QColor currentColor;
    QColor backgroundColor;
    int currentIndex;
    int interval;
    bool isFading;
    bool pulseDirection;
    bool isOn;
    Effect currentEffect;
    double currentOpacity;
    double targetOpacity;
    bool isDimming;
    bool isBrightening;

    void updateAnimation();

public slots:
    void togglePower(bool power);
};

#endif // LIGHTSTRIPWIDGET_H
