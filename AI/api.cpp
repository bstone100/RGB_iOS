#include "api.h"
#include "openai_message.h"
#include "openai_request.h"
#include "QDate"
#include "QJsonObject"
#include "QtCore/qjsonarray.h"
#include "QtCore/qjsondocument.h"
#include "../mainwindow.h"
#include "../widgets/lightstripwidget.h"

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
    // Set Solid Color
    QJsonObject setSolidColorParametersPropertiesObject {
        {"color", QJsonObject{{"type", "string"}, {"description", "Color for the light strip."}}}
    };
    QJsonObject setSolidColorParametersObject {
        {"type", "object"},
        {"properties", setSolidColorParametersPropertiesObject},
        {"required", QJsonArray{"color"}}
    };
    QJsonObject setSolidColorFunctionObject {
        {"name", "setSolidColor"},
        {"description", "Set the light strip to a solid color."},
        {"parameters", setSolidColorParametersObject}
    };
    QJsonObject setSolidColorDescription {
        {"type", "function"},
        {"function", setSolidColorFunctionObject}
    };
    toolList.append(APITool{&API::setSolidColor, setSolidColorDescription});

    // Start Fade On And Off
    QJsonObject startFadeOnAndOffParametersPropertiesObject {
        {"colors", QJsonObject{
           {"type", "array"},
           {"items", QJsonObject{{"type", "string"}, {"description", "Hexadecimal color code."}}},
           {"description", "List of colors."}
        }},
        {"interval", QJsonObject{{"type", "integer"}, {"description", "Time interval in milliseconds for each fade cycle."}}}
    };
    QJsonObject startFadeOnAndOffParametersObject {
        {"type", "object"},
        {"properties", startFadeOnAndOffParametersPropertiesObject},
        {"required", QJsonArray{"colors", "interval"}}
    };
    QJsonObject startFadeOnAndOffFunctionObject {
        {"name", "startFadeOnAndOff"},
        {"description", "Start fading the light strip on and off between specified colors."},
        {"parameters", startFadeOnAndOffParametersObject}
    };
    QJsonObject startFadeOnAndOffDescription {
        {"type", "function"},
        {"function", startFadeOnAndOffFunctionObject}
    };
    toolList.append(APITool{&API::startFadeOnAndOff, startFadeOnAndOffDescription});

    // Start Flash On And Off
    QJsonObject startFlashOnAndOffParametersPropertiesObject {
        {"colors", QJsonObject{
            {"type", "array"},
            {"items", QJsonObject{{"type", "string"}, {"description", "Hexadecimal color code."}}},
            {"description", "List of colors."}
        }},
        {"interval", QJsonObject{{"type", "integer"}, {"description", "Time interval in milliseconds for each flash cycle."}}}
    };
    QJsonObject startFlashOnAndOffParametersObject {
        {"type", "object"},
        {"properties", startFlashOnAndOffParametersPropertiesObject},
        {"required", QJsonArray{"colors", "interval"}}
    };
    QJsonObject startFlashOnAndOffFunctionObject {
        {"name", "startFlashOnAndOff"},
        {"description", "Start flashing the light strip on and off between specified colors."},
        {"parameters", startFlashOnAndOffParametersObject}
    };
    QJsonObject startFlashOnAndOffDescription {
        {"type", "function"},
        {"function", startFlashOnAndOffFunctionObject}
    };
    toolList.append(APITool{&API::startFlashOnAndOff, startFlashOnAndOffDescription});

    // Start Rotate With Fade
    QJsonObject startRotateWithFadeParametersPropertiesObject {
        {"colors", QJsonObject{
             {"type", "array"},
             {"items", QJsonObject{{"type", "string"}, {"description", "Hexadecimal color code."}}},
             {"description", "List of colors."}
         }},
        {"interval", QJsonObject{{"type", "integer"}, {"description", "Interval in milliseconds between rotations."}}}
    };
    QJsonObject startRotateWithFadeParametersObject {
        {"type", "object"},
        {"properties", startRotateWithFadeParametersPropertiesObject},
        {"required", QJsonArray{"colors", "interval"}}
    };
    QJsonObject startRotateWithFadeFunctionObject {
        {"name", "startRotateWithFade"},
        {"description", "Rotate the light colors with a fading effect between each color."},
        {"parameters", startRotateWithFadeParametersObject}
    };
    QJsonObject startRotateWithFadeDescription {
        {"type", "function"},
        {"function", startRotateWithFadeFunctionObject}
    };
    toolList.append(APITool{&API::startRotateWithFade, startRotateWithFadeDescription});

    // Start Rotate Without Fade
    QJsonObject startRotateWithoutFadeParametersPropertiesObject {
        {"colors", QJsonObject{
            {"type", "array"},
            {"items", QJsonObject{{"type", "string"}, {"description", "Hexadecimal color code."}}},
            {"description", "List of colors."}
        }},
        {"interval", QJsonObject{{"type", "integer"}, {"description", "Interval in milliseconds between color changes."}}}
    };
    QJsonObject startRotateWithoutFadeParametersObject {
        {"type", "object"},
        {"properties", startRotateWithoutFadeParametersPropertiesObject},
        {"required", QJsonArray{"colors", "interval"}}
    };
    QJsonObject startRotateWithoutFadeFunctionObject {
        {"name", "startRotateWithoutFade"},
        {"description", "Rotate the light colors abruptly without any fading effect."},
        {"parameters", startRotateWithoutFadeParametersObject}
    };
    QJsonObject startRotateWithoutFadeDescription {
        {"type", "function"},
        {"function", startRotateWithoutFadeFunctionObject}
    };
    toolList.append(APITool{&API::startRotateWithoutFade, startRotateWithoutFadeDescription});

    // Start Pulse
    QJsonObject startPulseParametersPropertiesObject {
        {"pulseColors", QJsonObject{
            {"type", "array"},
            {"items", QJsonObject{{"type", "string"}, {"description", "Hexadecimal color code."}}},
            {"description", "List of colors."}
        }},
        {"backgroundColor", QJsonObject{{"type", "string"}, {"description", "Hexadecimal color code for the background color."}}},
        {"interval", QJsonObject{{"type", "integer"}, {"description", "Duration in milliseconds it takes for a pulse to travel across the strip."}}},
        {"direction", QJsonObject{{"type", "boolean"}, {"description", "Direction of the pulse; true for one direction, false for the opposite."}}}
    };
    QJsonObject startPulseParametersObject {
        {"type", "object"},
        {"properties", startPulseParametersPropertiesObject},
        {"required", QJsonArray{"pulseColors", "backgroundColor", "interval", "direction"}}
    };
    QJsonObject startPulseFunctionObject {
        {"name", "startPulse"},
        {"description", "Initiate a pulsing effect across the light strip."},
        {"parameters", startPulseParametersObject}
    };
    QJsonObject startPulseDescription {
        {"type", "function"},
        {"function", startPulseFunctionObject}
    };
    toolList.append(APITool{&API::startPulse, startPulseDescription});

    // Dim Lights
    QJsonObject dimLightsParametersObject {
        {"type", "object"},
        {"properties", QJsonObject{}},
        {"required", QJsonArray{}}
    };
    QJsonObject dimLightsFunctionObject {
        {"name", "dimLights"},
        {"description", "Dim the lights by reducing the opacity."},
        {"parameters", dimLightsParametersObject}
    };
    QJsonObject dimLightsDescription {
        {"type", "function"},
        {"function", dimLightsFunctionObject}
    };
    toolList.append(APITool{&API::dimLights, dimLightsDescription});

    // Brighten Lights
    QJsonObject brightenLightsParametersObject {
        {"type", "object"},
        {"properties", QJsonObject{}},
        {"required", QJsonArray{}}
    };
    QJsonObject brightenLightsFunctionObject {
        {"name", "brightenLights"},
        {"description", "Increase the opacity to brighten the lights."},
        {"parameters", brightenLightsParametersObject}
    };
    QJsonObject brightenLightsDescription {
        {"type", "function"},
        {"function", brightenLightsFunctionObject}
    };
    toolList.append(APITool{&API::brightenLights, brightenLightsDescription});

    // Toggle Power
    QJsonObject togglePowerParametersPropertiesObject {
        {"power", QJsonObject{
                      {"type", "boolean"},
                      {"description", "Boolean value to turn the light strip on (true) or off (false)."}
                  }}
    };
    QJsonObject togglePowerParametersObject {
        {"type", "object"},
        {"properties", togglePowerParametersPropertiesObject},
        {"required", QJsonArray{"power"}}
    };
    QJsonObject togglePowerFunctionObject {
        {"name", "togglePower"},
        {"description", "Toggle the power state of the light strip."},
        {"parameters", togglePowerParametersObject}
    };
    QJsonObject togglePowerDescription {
        {"type", "function"},
        {"function", togglePowerFunctionObject}
    };
    toolList.append(APITool{&API::togglePower, togglePowerDescription});
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

    for (int i = 0; i < toolCalls.size(); i++) {
        QJsonObject toolCall = toolCalls.at(i).toObject();
        QJsonObject function = toolCall["function"].toObject();

        // call the function
        QString functionName = function["name"].toString();
        APITool functionToCall = API::getToolByName(functionName);

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


QString API::togglePower(const QJsonObject &jsonObject)
{
    bool power = jsonObject["power"].toBool();
    LightStripWidget::self()->togglePower(power);
    return "Power toggled successfully";
}

QString API::setSolidColor(const QJsonObject &jsonObject) {
    QColor color(jsonObject["color"].toString());
    LightStripWidget::self()->setSolidColor(color);
    return "Solid color set successfully.";
}

QString API::startFadeOnAndOff(const QJsonObject &jsonObject) {
    QList<QColor> colors = extractColorsFromJson(jsonObject, "colors");
    int interval = jsonObject["interval"].toInt();
    LightStripWidget::self()->startFadeOffAndOn(colors, interval);
    return "Fade on and off effect started.";
}

QString API::startFlashOnAndOff(const QJsonObject &jsonObject) {
    QList<QColor> colors = extractColorsFromJson(jsonObject, "colors");
    int interval = jsonObject["interval"].toInt();
    LightStripWidget::self()->startFlashOffAndOn(colors, interval);
    return "Flash on and off effect started.";
}

QString API::startRotateWithFade(const QJsonObject &jsonObject) {
    QList<QColor> colors = extractColorsFromJson(jsonObject, "colors");
    int interval = jsonObject["interval"].toInt();
    LightStripWidget::self()->startRotateWithFade(colors, interval);
    return "Rotate with fade effect started.";
}

QString API::startRotateWithoutFade(const QJsonObject &jsonObject) {
    QList<QColor> colors = extractColorsFromJson(jsonObject, "colors");
    int interval = jsonObject["interval"].toInt();
    LightStripWidget::self()->startRotateWithoutFade(colors, interval);
    return "Rotate without fade effect started.";
}

QString API::startPulse(const QJsonObject &jsonObject) {
    QList<QColor> pulseColors = extractColorsFromJson(jsonObject, "pulseColors");
    QColor backgroundColor(jsonObject["backgroundColor"].toString());
    int interval = jsonObject["interval"].toInt();
    bool direction = jsonObject["direction"].toBool();
    LightStripWidget::self()->startPulse(pulseColors, backgroundColor, interval, direction);
    return "Pulse effect started.";
}

QString API::dimLights(const QJsonObject &jsonObject) {
    LightStripWidget::self()->dimLights();
    return "Dimming initiated.";
}

QString API::brightenLights(const QJsonObject &jsonObject) {
    LightStripWidget::self()->brightenLights();
    return "Brightening initiated.";
}



QList<QColor> API::extractColorsFromJson(const QJsonObject &jsonObject, const QString &key) {
    QList<QColor> colors;
    QJsonArray jsonColors = jsonObject[key].toArray();
    for (const auto &jsonVal : jsonColors) {
    colors.append(QColor(jsonVal.toString()));
    }
    return colors;
}












