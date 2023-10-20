// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include "audiorecorder.h"
#include "audiolevel.h"

#include <QAudioBuffer>
#include <QAudioDevice>
#include <QAudioInput>
#include <QDir>
#include <QFileDialog>
#include <QImageCapture>
#include <QMediaDevices>
#include <QMediaFormat>
#include <QMediaRecorder>
#include <QMimeType>
#include <QStandardPaths>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMessageBox>
#include <QApplication>

#if QT_CONFIG(permissions)
  #include <QPermission>
#endif


AudioRecorder::AudioRecorder()
{
    // audio input initialization
    init();
}

void AudioRecorder::init()
{
#if QT_CONFIG(permissions)
    QMicrophonePermission microphonePermission;
    switch (qApp->checkPermission(microphonePermission)) {
    case Qt::PermissionStatus::Undetermined:
        qApp->requestPermission(microphonePermission, this, &AudioRecorder::init);
        return;
    case Qt::PermissionStatus::Denied:
        QMessageBox::warning(NULL, "Permission Error", "Microphone permission is not granted!");
        return;
    case Qt::PermissionStatus::Granted:
        break;
    }
#endif

    m_audioRecorder = new QMediaRecorder(this);
    m_captureSession.setRecorder(m_audioRecorder);
    m_captureSession.setAudioInput(new QAudioInput(this));


    QString tempDir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QString tempFilePath = tempDir + "/speech.wav";

    m_audioRecorder->setOutputLocation(QUrl::fromLocalFile(tempFilePath));
}

void AudioRecorder::toggleRecord()
{
    if (m_audioRecorder->recorderState() == QMediaRecorder::StoppedState) {

#if QT_CONFIG(permissions)
        QMicrophonePermission microphonePermission;
        switch (qApp->checkPermission(microphonePermission)) {
        case Qt::PermissionStatus::Undetermined:
            qApp->requestPermission(microphonePermission, this, &AudioRecorder::toggleRecord);
            return;
        case Qt::PermissionStatus::Denied:
            QMessageBox::warning(NULL, "Permission Error", "Microphone permission is not granted!");
            return;
        case Qt::PermissionStatus::Granted:
            break;
        }
#endif

        m_captureSession.audioInput()->setDevice(QMediaDevices::defaultAudioInput());

        QMediaFormat format;
        format.setFileFormat(QMediaFormat::Wave);
        format.setAudioCodec(QMediaFormat::AudioCodec::Wave);
        m_audioRecorder->setMediaFormat(format);
        m_audioRecorder->setAudioSampleRate(16000);
        m_audioRecorder->setAudioBitRate(128000);
        m_audioRecorder->setAudioChannelCount(1);
        m_audioRecorder->setQuality(QMediaRecorder::HighQuality);
        m_audioRecorder->setEncodingMode(QMediaRecorder::ConstantBitRateEncoding);

        m_audioRecorder->record();
    } else {
        m_audioRecorder->stop();
    }
}

void AudioRecorder::togglePause()
{
    if (m_audioRecorder->recorderState() != QMediaRecorder::PausedState)
        m_audioRecorder->pause();
    else
        m_audioRecorder->record();
}


