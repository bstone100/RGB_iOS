#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QSettings>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onApiKeyButtonClicked();
    void onDarkModeButtonClicked();

private:
    void saveApiKey();
    void loadApiKey();
    void updateApiKeyButtonLabel();

    void saveDarkMode();
    void loadDarkMode();
    void setDarkMode(bool darkMode);

    QWidget *centralWidget;
    QVBoxLayout *layout;
    QPushButton *apiKeyButton;
    QPushButton *darkModeButton; // Added dark mode button
    QString apiKey;
    bool isDarkMode; // Track the current mode

    static const QString apiKeySettingsKey;
    static const QString isDarkModeSettingsKey;
};

#endif // MAINWINDOW_H
