#ifndef MICROPHONEWIDGET_H
#define MICROPHONEWIDGET_H

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

signals:
    void clicked();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;  // Handle mouse press events
    void mouseReleaseEvent(QMouseEvent *event) override;  // Handle mouse release events

private:
    static MicrophoneWidget *singleton;

    QPixmap micImage;
    float currentLevel;

    int imageSize;

    bool isPressed;
};

#endif // MICROPHONEWIDGET_H
