#include "microphonewidget.h"
#include <QPainter>
#include <QTimer>
#include <QSize>
#include "../mainwindow.h"
#include "../audio/audiotranscriptionmanager.h"

MicrophoneWidget *MicrophoneWidget::singleton = NULL;

MicrophoneWidget::MicrophoneWidget(QWidget *parent) : QWidget(parent), currentLevel(0.0)
{
    if (!singleton) {
        singleton = this;
    }


    setFixedSize(300, 300);

    QPixmap originalImage(":/images/mic_1107.png");

    qreal ratio = devicePixelRatioF();
    imageSize = 0.8 * width();

    micImage = originalImage.scaled(imageSize * ratio, imageSize * ratio, Qt::KeepAspectRatio, Qt::SmoothTransformation); // Adjust the size as needed
    micImage.setDevicePixelRatio(ratio);
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
    float minRadius = imageSize / 2 - 5;
    float maxRadius = width() / 2;
    int levelRadius = qBound(minRadius, minRadius + currentLevel * (maxRadius - minRadius), maxRadius);

    QColor color(255, 255, 255, 100); // Semi-transparent white
    painter.setPen(Qt::NoPen);
    painter.setBrush(color);

    painter.drawEllipse(QPoint(width() / 2, height() / 2), levelRadius, levelRadius);

    // Draw the pixmap
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














