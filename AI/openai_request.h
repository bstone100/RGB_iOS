#pragma once

#include <QObject>
#include <QNetworkAccessManager>

#include "openai_message.h"


class OpenAIRequest : public QObject
{
    Q_OBJECT

public:
    explicit OpenAIRequest(QObject *parent = nullptr);
    virtual ~OpenAIRequest();

    enum class RequestStatus {
        Idle,
        InProgress,
        Error,
        Success
    };

public slots:
    QString accessToken() const;
    void setAccessToken(const QString& accessToken);

    QString model() const;
    void setModel(const QString& model);

    QString prompt() const;
    void setPrompt(const QString& prompt);

    QString filePath() const;
    void setFilePath(const QString& filePath);

    QString generatedText() const;
    QString errorString() const;
    RequestStatus status() const;

    int maxTokens() const;
    void setMaxTokens(int maxTokens);

    double temperature() const;
    void setTemperature(double temperature);

    double topP() const;
    void setTopP(double topP);

    double frequencyPenalty() const;
    void setFrequencyPenalty(double frequencyPenalty);

    double presencePenalty() const;
    void setPresencePenalty(double presencePenalty);

    QList<OpenAIMessage*> messages() const;
    void setMessages(const QList<OpenAIMessage *> &newMessages);

    // execute a request
    void execute();

signals:
    // Signal emitted when the request is finished successfully
    void requestFinished(const QString& generatedText);

    // Signal emitted when the request encounters an error
    void requestError(const QString& errorString);

    void accessTokenChanged();
    void modelChanged();
    void promptChanged();
    void filePathChanged();
    void generatedTextChanged();
    void errorStringChanged();
    void statusChanged();
    void maxTokensChanged();
    void temperatureChanged();
    void topPChanged();
    void frequencyPenaltyChanged();
    void presencePenaltyChanged();
    void messagesChanged();

private:
    QNetworkAccessManager *m_networkAccessManager;
    QString m_accessToken;
    QString m_model;
    QString m_prompt;
    QString m_filePath;
    QString m_generatedText;
    QString m_errorString;
    RequestStatus m_status;
    int m_maxTokens;
    double m_temperature;
    double m_topP;
    double m_frequencyPenalty;
    double m_presencePenalty;
    QList<OpenAIMessage*> m_messages;

    void sendRequest();
    void sendChatRequest();
    void sendMultiPartRequest();
};
