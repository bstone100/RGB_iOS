#include "api.h"
#include "openai_message.h"
#include "openai_request.h"
#include "QDate"
#include "QJsonObject"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsondocument.h"
#include "../mainwindow.h"

QList<APITool> API::toolList = {};

API::API()
{

}

QList<APITool> API::tools()
{
    if (toolList.isEmpty()) {
        generateTools();
    }
    return toolList;
}

void API::generateTools()
{
    // put json descriptions here
}

QJsonArray API::getToolsJsonArray()
{
    QJsonArray toolArray;
    foreach (APITool tool, tools()) {
        toolArray.append(tool.description);
    }
    return toolArray;
}

APITool API::getToolByName(const QString &name)
{
    foreach (APITool tool, tools()) {
        if (tool.getName() == name) {
            return tool;
        }
    }
    return APITool();
}

void API::processToolCalls(const QJsonArray &toolCalls, OpenAIRequest *chatRequest)
{
    if (toolCalls.isEmpty()) return;

    bool onlyGettingEvents = true;
    for (int i = 0; i < toolCalls.size(); i++) {
        QJsonObject toolCall = toolCalls.at(i).toObject();
        QJsonObject function = toolCall["function"].toObject();

        // call the function
        QString functionName = function["name"].toString();
        APITool functionToCall = API::getToolByName(functionName);
        if (functionName != "getEventsInRange" && functionName != "getContextForRange") {
            onlyGettingEvents = false;
        }

        QString argumentsStr = function["arguments"].toString();
        QJsonDocument doc = QJsonDocument::fromJson(argumentsStr.toUtf8());
        QJsonObject functionArgs = doc.object();

        printToolCall(functionName, functionArgs);

        QString functionResponse;
        if (functionToCall.isValid()) {
            functionResponse = functionToCall.execute(functionArgs);
        } else {
            functionResponse = functionName + " is not a valid function.";
        }
        qDebug() << functionResponse;

        // append the function response to conversation
        OpenAIMessage *toolMessage = new OpenAIMessage(functionResponse, OpenAIMessage::Role::Tool);
        toolMessage->setTool_call_id(toolCall["id"].toString());
        chatRequest->addMessage(toolMessage);
    }

    // request that the responses be summarized or that more function calls be made
    chatRequest->execute();

    MainWindow::self()->saveSettings();
}

void API::printToolCall(const QString &name, const QJsonObject &args)
{
        QStringList debugStringList{name, "("};
        for (auto it = args.begin(); it != args.end(); ++it) {
        debugStringList << it.value().toVariant().typeName() << " " << it.value().toString() << ", ";
        }
        debugStringList << ")";
        qDebug() << debugStringList.join("");
}












