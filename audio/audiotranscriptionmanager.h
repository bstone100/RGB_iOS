#ifndef AUDIOTRANSCRIPTIONMANAGER_H
#define AUDIOTRANSCRIPTIONMANAGER_H

#include <QObject>
#include "QTimer"

class AudioTranscriptionManager : public QObject {
    Q_OBJECT

public:
    AudioTranscriptionManager();
    static AudioTranscriptionManager *self();

    void start();
    void stop();

signals:
    void transcriptionUpdated(const QString &transcription);
    void silenceDetected();
    void levelCalculated(const float level);
    void timeLimitReached();

private:
    static AudioTranscriptionManager *singleton;
    void init();

    void updateLevel();
    QTimer updateLevelTimer;
};

#endif // AUDIOTRANSCRIPTIONMANAGER_H
