#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QBluetoothSocket>

class BluetoothManager : public QObject
{
    Q_OBJECT
public:
    explicit BluetoothManager(QObject *parent = nullptr);
    ~BluetoothManager();

    static BluetoothManager *self();

    void startDeviceDiscovery();
    void connectToDevice(const QBluetoothDeviceInfo &deviceInfo);
    void sendJsonObject(const QJsonObject &jObj);

    void sendCurrentState();

signals:
    void deviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void connected();
    void disconnected();
    void errorOccurred(const QString &errorMessage);

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &deviceInfo);
    void onConnected();
    void onDisconnected();
    void onReadReady();

private:
    static BluetoothManager *singleton;

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothSocket *bluetoothSocket;
};

#endif // BLUETOOTHMANAGER_H
