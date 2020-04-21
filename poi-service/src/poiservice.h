#pragma once

#include <QObject>
#include <QString>
#include <QJsonValue>

#include "webclient.h"
#include <poi.h>

class PoiService: public QObject
{
    Q_OBJECT

    WebClient webClient;
    POI *poiapi;
private slots:
    void onConnected();
    void onRequestReceived(uint32_t requestId, const QString &requestType, const QJsonValue &requestParameter);
public:
    PoiService(POI *poi, QObject *parent);
    ~PoiService();
};
