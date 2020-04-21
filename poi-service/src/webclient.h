#pragma once

#include <QObject>
#include <QJsonValue>
#include <QNetworkAccessManager>

class WebClient: QObject
{
    Q_OBJECT

    QNetworkAccessManager nm;
public:
    WebClient(QObject *parent);
public slots:
    QJsonValue externalOnlineRequestHttpGetJson(const QJsonValue &request);
};
