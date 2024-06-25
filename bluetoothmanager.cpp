#include "bluetoothmanager.h"
#include "QtCore/qjsondocument.h"
#include <QBluetoothServiceInfo>
#include "widgets/lightstripwidget.h"
#include "QJsonObject"
#include "mainwindow.h"

BluetoothManager* BluetoothManager::singleton = nullptr;

BluetoothManager::BluetoothManager(QObject* parent)
    : QObject(parent),
    discoveryAgent(new QBluetoothDeviceDiscoveryAgent(this)),
    bluetoothSocket(new QBluetoothSocket(QBluetoothServiceInfo::RfcommProtocol, this))
{
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
            this, &BluetoothManager::onDeviceDiscovered);
    connect(discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished,
            discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deleteLater);
    connect(bluetoothSocket, &QBluetoothSocket::connected,
            this, &BluetoothManager::onConnected);
    connect(bluetoothSocket, &QBluetoothSocket::disconnected,
            this, &BluetoothManager::onDisconnected);
    connect(bluetoothSocket, &QBluetoothSocket::readyRead,
            this, &BluetoothManager::onReadReady);
}

BluetoothManager* BluetoothManager::self()
{
    if (!singleton)
        singleton = new BluetoothManager();
    return singleton;
}

BluetoothManager::~BluetoothManager()
{
    delete bluetoothSocket;
    delete discoveryAgent;
}

void BluetoothManager::startDeviceDiscovery()
{
    discoveryAgent->start();
}

void BluetoothManager::connectToDevice(const QBluetoothDeviceInfo& deviceInfo)
{
    bluetoothSocket->connectToService(deviceInfo.address(), 1); // Assume channel 1 for RFCOMM
}

void BluetoothManager::sendJsonObject(const QJsonObject& jObj)
{
    if (bluetoothSocket->state() == QBluetoothSocket::SocketState::ConnectedState) {
        QJsonDocument doc(jObj);
        QByteArray data = doc.toJson();
        bluetoothSocket->write(data);
    }
}

void BluetoothManager::sendCurrentState()
{
//    auto jObj = LightStripWidget::self()->getJsonObject();
    sendJsonObject(LightStripWidget::self()->getJsonObject());
//    MainWindow::self()->dumpJsonToFile(jObj, "currentState.json");
}


void BluetoothManager::onDeviceDiscovered(const QBluetoothDeviceInfo& deviceInfo)
{
    emit deviceDiscovered(deviceInfo);
}

void BluetoothManager::onConnected()
{
    emit connected();
}

void BluetoothManager::onDisconnected()
{
    emit disconnected();
}

void BluetoothManager::onReadReady()
{
    QByteArray data = bluetoothSocket->readAll();
    // Handle data
    // For example, emit a signal with the received data
}

