#include "microphonewidget.h"
#include <QPainter>
#include <QTimer>
#include <QSize>
#include "../mainwindow.h"
#include "lightstripwidget.h"

MicrophoneWidget *MicrophoneWidget::singleton = NULL;

MicrophoneWidget::MicrophoneWidget(QWidget *parent) : QWidget(parent), currentLevel(0.0)
{
    if (!singleton) {
        singleton = this;
    }

    setFixedSize(300, 300);

    originalImage = QPixmap(":/images/mic_1107.png");

    collapsedSize = 150;
    expandedSize = 250;

    qreal ratio = devicePixelRatioF();

    micImage = originalImage.scaled(collapsedSize * ratio, collapsedSize * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation); // Adjust the size as needed
    micImage.setDevicePixelRatio(ratio);


    updateAnimationTimer.setInterval(10);
    updateAnimationTimer.setSingleShot(false);
    connect(&updateAnimationTimer, &QTimer::timeout, this, &MicrophoneWidget::updateAnimation);

    currentTime = 0;
    cycleTime = 200;
    progress = 0.0;

    isExpanding = false;
    isCollapsing = false;
}

MicrophoneWidget::~MicrophoneWidget()
{

}

MicrophoneWidget *MicrophoneWidget::self()
{
    if (!singleton) {
        singleton = new MicrophoneWidget(MainWindow::self());
    }
    return singleton;
}

void MicrophoneWidget::setLevel(float level)
{
    currentLevel = level;
    update();
}

void MicrophoneWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw the animated circle
    float minRadius = currentSize / 2 - 5;
    float maxRadius = width() / 2;
    int levelRadius = qBound(minRadius, minRadius + currentLevel * (maxRadius - minRadius), maxRadius);

    QColor color(255, 255, 255, 100); // Semi-transparent white
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);

    painter.drawEllipse(QPoint(width() / 2, height() / 2), levelRadius, levelRadius);

    // Draw the pixmap in the middle of the widget
    qreal ratio = devicePixelRatioF();
    painter.drawPixmap(QRect((width() - micImage.width() / ratio) / 2,
                             (height() - micImage.height() / ratio) / 2,
                             micImage.width() / ratio,
                             micImage.height() / ratio), micImage);
}

void MicrophoneWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        isPressed = true;
    }
}

void MicrophoneWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isPressed) {
        isPressed = false;
        emit clicked();  // Emit the clicked signal
    }
}

void MicrophoneWidget::expand()
{
    if (isCollapsing) {
        updateAnimationTimer.stop();
        currentTime = cycleTime - currentTime;
        isCollapsing = false;
    }

    isExpanding = true;
    updateAnimationTimer.start();
}

void MicrophoneWidget::collapse()
{
    if (isExpanding) {
        updateAnimationTimer.stop();
        currentTime = cycleTime - currentTime;
        isExpanding = false;
    }

    isCollapsing = true;
    updateAnimationTimer.start();
}

void MicrophoneWidget::updateAnimation()
{
    currentTime += updateAnimationTimer.interval();
    progress = qBound(0.0, (float)currentTime / cycleTime, 1.0);

    // scale image based on progress
    if (isExpanding) {
        currentSize = LightStripWidget::linearlyInterpolate(collapsedSize, expandedSize, progress);
    } else {
        currentSize = LightStripWidget::linearlyInterpolate(expandedSize, collapsedSize, progress);
    }
    qreal ratio = devicePixelRatioF();
    micImage = originalImage.scaled(currentSize * ratio, currentSize * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    micImage.setDevicePixelRatio(ratio);
    update();

    if (currentTime >= cycleTime) {
        updateAnimationTimer.stop();
        isExpanding = false;
        isCollapsing = false;
        currentTime = 0;
    }
}











