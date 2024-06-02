#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QSettings>
#include <QRadioButton>
#include <QTouchEvent>
#include "QTextEdit"
#include "QStackedWidget"
#include "qpropertyanimation.h"
#include "QQueue"
#include "QTableView"
#include "QTimer"
#include "QAudioSink"

class OpenAIRequest;
class AudioRecorder;
class SvgButton;
class ResizingTextEdit;
class AudioLevel;
class ResizingComboBox;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static MainWindow *self();

    void sendChat();

    void saveSettings();
    void loadSettings();

    static QString version;
    static QString currentPath;

    static QColor lightColor;
    static QColor lightMidColor;
    static QColor darkMidColor;
    static QColor darkColor;

    bool isDarkModeOn(){return isDarkMode;}
    bool isSystemDark();
    void handleThemeChange(bool isDarkMode);

    QPropertyAnimation *fadeInWidget(QWidget *widget, int duration);
    QPropertyAnimation *fadeOutWidget(QWidget *widget, int duration);
    void fadeInWidgets(QList<QWidget *> widgets, int duration);
    void fadeOutWidgets(QList<QWidget *> widgets, int duration);
    void setWidgetOpacity(QWidget *widget, double opacity);
    double getWidgetOpacity(QWidget *widget);

    int getCurrentTranscriptionWordCount();


protected:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    static MainWindow *singleton;

    bool settingsLoaded = false;
    bool onboarded = false;

    void setDarkMode(bool isDarkMode);

    QSettings *settings;

    QWidget *centralWidget;
    QVBoxLayout *layout;

    ResizingComboBox *themeComboBox;
    ResizingComboBox *effectComboBox;

    QString apiKey;
    bool isDarkMode;
    bool isAutoTheme;

    OpenAIRequest *chatRequest;

    // gestures

    void touchEvent(QTouchEvent *event);

    enum Gesture {
        SidePanel = 0,
        Undefined
    };

    Gesture currentGesture = Undefined;

    // audio
    ResizingTextEdit *transcriptionTextEdit;

    void updateTranscriptionText(QString text);
    void handleAudioTimeLimit();

    QString transcriptionBeginning;
    QString transcriptionCurrent;

};

#endif // MAINWINDOW_H








