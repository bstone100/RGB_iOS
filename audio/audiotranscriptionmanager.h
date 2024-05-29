#ifndef AUDIOTRANSCRIPTIONMANAGER_H
#define AUDIOTRANSCRIPTIONMANAGER_H

#include <QObject>

class AudioTranscriptionManager : public QObject {
    Q_OBJECT

public:
    AudioTranscriptionManager();
    static AudioTranscriptionManager *self();

    void start();
    void stop();

    static void transcriptionUpdated(const char *result);

signals:
    void transcriptionReceived(const QString &transcription);

private:
    static AudioTranscriptionManager *singleton;
    void init();
};

#endif // AUDIOTRANSCRIPTIONMANAGER_H
