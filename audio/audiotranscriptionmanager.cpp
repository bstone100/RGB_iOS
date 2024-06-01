#include "audiotranscriptionmanager.h"
#include <QString>
#include "QPermission"
#include <QMessageBox>
#include <QApplication>
#include "../widgets/microphonewidget.h"

// android and others will have equivalent file
#include "whisperinterface.h"

AudioTranscriptionManager *AudioTranscriptionManager::singleton = NULL;

AudioTranscriptionManager::AudioTranscriptionManager() {
    if (!singleton) {
        singleton = this;
    }

    init();

    setupAudioCapture();

    updateLevelTimer.setSingleShot(false);
    updateLevelTimer.setInterval(10);

    connect(&updateLevelTimer, &QTimer::timeout, this, &AudioTranscriptionManager::updateLevel);
}

AudioTranscriptionManager *AudioTranscriptionManager::self()
{
    if (!singleton) {
        singleton = new AudioTranscriptionManager();
    }
    return singleton;
}

void AudioTranscriptionManager::init()
{
#if QT_CONFIG(permissions)
    QMicrophonePermission microphonePermission;
    switch (qApp->checkPermission(microphonePermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(microphonePermission, this, &AudioTranscriptionManager::init);
        return;
    case Qt::PermissionStatus::Denied:
        QMessageBox::warning(NULL, "Permission Error", "Microphone permission is not granted!");
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }
#endif
}

void AudioTranscriptionManager::start() {
    startAudioCapture();
    updateLevelTimer.start();
}

void AudioTranscriptionManager::stop() {
    updateLevelTimer.stop();
    stopAudioCapture();
}

void AudioTranscriptionManager::updateLevel()
{
    float level = getCurrentLevel();
    level = qBound(0.0, level * 3, 1.0); // make number bigger
    MicrophoneWidget::self()->setLevel(level);
}










