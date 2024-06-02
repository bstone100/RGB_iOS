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
        SolidColor = 0,
        FadeOffAndOn,
        FlashOffAndOn,
        RotateWithFade,
        RotateWithoutFade,
        Pulse,

        Min = SolidColor,
        Max = Pulse
    };

    static QString effectToString(Effect effect);
    static Effect effectFromString(QString text);
    static QStringList getEffectList();

    QJsonObject getJsonObject();
    void loadJsonObject(const QJsonObject &jObj);

    void startEffect(Effect effect);
    void startTestEffect(Effect effect);

    void setSolidColor(const QColor &color);
    void dimLights();
    void brightenLights();

    void startFadeOffAndOn(const QList<QColor> &colors, int interval);
    void startFlashOffAndOn(const QList<QColor> &colors, int interval);
    void startRotateWithFade(const QList<QColor> &colors, int interval);
    void startRotateWithoutFade(const QList<QColor> &colors, int interval);
    void startPulse(const QList<QColor> &pulseColors, const QColor &backgroundColor, int interval, bool direction);
    void paintEvent(QPaintEvent *event) override;

    Effect getCurrentEffect() const;

private:
    static LightStripWidget *singleton;

    QTimer *timer;
    int currentTime;
    int cycleTime;
    float progress;

    QList<QColor> colors;
    QColor currentColor;
    QColor backgroundColor;
    int currentIndex;

    bool isFading;
    bool pulseDirection;
    bool isOn;
    Effect currentEffect;

    bool isDim;
    bool isDimming;
    bool isBrightening;

    void updateAnimation();

    static QColor blendColors(const QColor &startColor, const QColor &endColor, double progress);
    static double linearlyInterpolate(double startVal, double endVal, double progress);

public slots:
    void togglePower(bool power);
};

#endif // LIGHTSTRIPWIDGET_H
