#ifndef MICROPHONEWIDGET_H
#define MICROPHONEWIDGET_H

#include "QtCore/qpropertyanimation.h"
#include <QWidget>
#include <QPainter>
#include <QTimer>

class MicrophoneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MicrophoneWidget(QWidget *parent = nullptr);
    ~MicrophoneWidget();

    static MicrophoneWidget *self();

    void setLevel(float level);

    void expand();
    void collapse();

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;  // Handle mouse press events
    void mouseReleaseEvent(QMouseEvent *event) override;  // Handle mouse release events

private:
    static MicrophoneWidget *singleton;

    QPixmap originalImage;
    QPixmap micImage;
    float currentLevel;

    bool isPressed;

    QTimer updateAnimationTimer;
    int currentTime;
    int cycleTime;
    float progress;

    bool isExpanding;
    bool isCollapsing;

    int collapsedSize;
    int expandedSize;
    int currentSize;

    void updateAnimation();

};

#endif // MICROPHONEWIDGET_H



