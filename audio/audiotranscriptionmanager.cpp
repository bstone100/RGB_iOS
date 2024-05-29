#include "audiotranscriptionmanager.h"
#include <QString>
#include "QPermission"
#include <QMessageBox>
#include <QApplication>

// android and others will have equivalent file
#include "whisperinterface.h"

AudioTranscriptionManager *AudioTranscriptionManager::singleton = NULL;

AudioTranscriptionManager::AudioTranscriptionManager() {
    if (!singleton) {
        singleton = this;
    }

    init();

    registerTranscriptionUpdateCallback(transcriptionUpdated);
    setupAudioCapture();
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
}

void AudioTranscriptionManager::stop() {
    stopAudioCapture();
}

void AudioTranscriptionManager::transcriptionUpdated(const char *result) {
    // Ensure this is thread-safe if called from a different thread
    emit AudioTranscriptionManager::self()->transcriptionReceived(QString::fromUtf8(result));
}
