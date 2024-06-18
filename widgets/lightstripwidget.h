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
        OffAndOn,
        Rotate,
        Pulse,

        Min = SolidColor,
        Max = Pulse
    };

    static QString effectToString(Effect effect);
    static Effect effectFromString(QString text);
    static QStringList getEffectList();

    QJsonObject getJsonObject();
    void loadJsonObject(const QJsonObject &jObj);

    void togglePower(bool power);

    void startEffect(Effect effect);
    void startTestEffect(Effect effect);

    void setSolidColor(const QColor &color);
    void dimLights();
    void brightenLights();

    void startOffAndOn(const QList<QColor> &colors, int interval, bool fade);
    void startRotate(const QList<QColor> &colors, int interval, bool fade);
    void startPulse(const QList<QColor> &pulseColors, const QColor &backgroundColor, int interval, bool direction);
    void paintEvent(QPaintEvent *event) override;

    Effect getCurrentEffect() const;

    static QColor blendColors(const QColor &startColor, const QColor &endColor, double progress);
    static double linearlyInterpolate(double startVal, double endVal, double progress);

private:
    static LightStripWidget *singleton;

    QTimer *timer;
    int currentTime;
    int cycleTime;
    float progress;

    bool fadeBetweenColors;
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
};

#endif // LIGHTSTRIPWIDGET_H
