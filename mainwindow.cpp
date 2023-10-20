#include "mainwindow.h"
#include <QApplication>
#include <QFile>

const QString MainWindow::apiKeySettingsKey = "apiKey";
const QString MainWindow::isDarkModeSettingsKey = "isDarkMode";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    layout = new QVBoxLayout(centralWidget);
    apiKeyButton = new QPushButton(this);
    darkModeButton = new QPushButton(this);

    layout->addWidget(apiKeyButton);
    layout->addWidget(darkModeButton);

    connect(apiKeyButton, &QPushButton::clicked, this, &MainWindow::onApiKeyButtonClicked);
    connect(darkModeButton, &QPushButton::clicked, this, &MainWindow::onDarkModeButtonClicked);

    // Load dark mode by default if no value saved in QSettings
    loadDarkMode();
    setDarkMode(isDarkMode); // Apply the mode

    loadApiKey();
    updateApiKeyButtonLabel();
}

MainWindow::~MainWindow()
{
    saveApiKey();
    saveDarkMode();
}

void MainWindow::onApiKeyButtonClicked()
{
    QString prompt = "Enter your OpenAI API Key:";
    QString currentApiKey = apiKey;

    bool ok;
    QString newApiKey = QInputDialog::getText(this, "API Key", prompt, QLineEdit::Normal, currentApiKey, &ok);
    if (!ok) return;

    apiKey = newApiKey;
    saveApiKey();

    updateApiKeyButtonLabel();
}

void MainWindow::onDarkModeButtonClicked()
{
    isDarkMode = !isDarkMode; // Toggle dark mode
    saveDarkMode();
    setDarkMode(isDarkMode); // Apply the selected mode
}

void MainWindow::setDarkMode(bool darkMode)
{
    QString path = darkMode ? ":/style/rgbDark.qss" : ":/style/rgbLight.qss";

    QFile file(path);
    if (file.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = file.readAll();
        qApp->setStyleSheet(styleSheet);
        qApp->processEvents();
    }

    if (darkMode) {
        darkModeButton->setText("Switch to Light Mode");
    } else {
        darkModeButton->setText("Switch to Dark Mode");
    }
}

void MainWindow::saveApiKey()
{
    QSettings settings;
    settings.setValue(apiKeySettingsKey, apiKey);
}

void MainWindow::loadApiKey()
{
    QSettings settings;
    apiKey = settings.value(apiKeySettingsKey).toString();
}

void MainWindow::saveDarkMode()
{
    QSettings settings;
    settings.setValue(isDarkModeSettingsKey, isDarkMode);
}

void MainWindow::loadDarkMode()
{
    QSettings settings;
    isDarkMode = settings.value(isDarkModeSettingsKey).toBool();
}

void MainWindow::updateApiKeyButtonLabel()
{
    if (apiKey.isEmpty()) {
        apiKeyButton->setText("Add API Key");
    } else {
        apiKeyButton->setText("Change API Key");
    }
}
