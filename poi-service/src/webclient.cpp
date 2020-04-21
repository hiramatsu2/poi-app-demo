#include "webclient.h"

#include <QEventLoop>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QJsonArray>

static inline bool qjCheckObject(const QJsonValue &jv, QJsonObject &jo) {
    if (!jv.isObject())
        return false;
    jo = jv.toObject();
    return true;
}

static inline bool qjCheckObject(const QJsonValue &jv, QJsonArray &ja) {
    if (!jv.isArray())
        return false;
    ja = jv.toArray();
    return true;
}

static inline bool qjGetFieldValue(const QJsonValue &jv, const QString &key, QJsonValue &jo) {
    QJsonObject tmp;
    if (!qjCheckObject(jv, tmp) || (tmp.constFind(key) == tmp.constEnd()))
        return false;
    jo = tmp[key];
    return true;
}

template <typename T>
static inline bool qjGetFieldValue(const T &jv, const QString &key, QString &str) {
    QJsonValue tmp;
    if (!qjGetFieldValue(jv, key, tmp) || !tmp.isString())
        return false;
    str = tmp.toString();
    return true;
}

template <typename T>
static inline bool qjGetFieldValue(const T &jv, const QString &key, QJsonArray &ja) {
    QJsonValue tmp;
    if (!qjGetFieldValue(jv, key, tmp) || !tmp.isArray())
        return false;
    ja = tmp.toArray();
    return true;
}

WebClient::WebClient(QObject *parent):
    QObject(parent),
    nm{}
{
}

QJsonValue WebClient::externalOnlineRequestHttpGetJson(const QJsonValue &request)
{
    QJsonObject jReq;
    QString href;
    if (!qjCheckObject(request, jReq) || !qjGetFieldValue(jReq, "href", href)) {
        return QJsonValue();
    }

    QNetworkRequest hreq(href);

    QJsonArray http_header;
    if (qjGetFieldValue(jReq, "http-header", http_header)) {
        for (QJsonValueRef f : http_header) {
            QJsonArray fa;
            if (!qjCheckObject(f, fa) || fa.size() < 2 || !fa[0].isString() || !fa[1].isString())
                continue;
            hreq.setRawHeader(fa[0].toString().toLocal8Bit(), fa[1].toString().toLocal8Bit());
        }
    }

    QEventLoop loop;
    QObject::connect(&nm, &QNetworkAccessManager::finished, &loop, &QEventLoop::quit);
    QNetworkReply *reply = nm.get(hreq);
    loop.exec();

    if ((reply->error() != QNetworkReply::NetworkError::NoError) ||
            (reply->header(QNetworkRequest::KnownHeaders::ContentTypeHeader).toString() != "application/json")) {
        reply->deleteLater();
        return QJsonValue();
    }

    QJsonDocument result = QJsonDocument::fromJson(reply->readAll());
    reply->deleteLater();
    if (result.isNull() || !result.isObject()) {
        return QJsonValue();
    } else {
        return result.object();
    }
}
