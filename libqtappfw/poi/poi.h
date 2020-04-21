 /*
 * Copyright (C) 2019 Konsulko Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef POI_H
#define POI_H

#include <QDebug>
#include <QObject>
#include <QJsonArray>
#include <QtQml/QQmlListProperty>

#include "messageengine.h"
#include "poimessage.h"
#include "responsemessage.h"

static constexpr char POI_ParameterKeyRequestId[] = "request-id";
static constexpr char POI_ParameterKeyRequestObject[] = "request-object";
static constexpr char POI_ParameterKeyRequestType[] = "request-type";
static constexpr char POI_ParameterKeyRequestParameter[] = "request-parameter";
static constexpr char POI_ParameterKeyResponseObject[] = "response-object";
static constexpr char POI_ParameterValueRequestTypeExternalOnlineRequest[] = "external-online-request-http-get-json";

class POI : public QObject
{
    Q_OBJECT

    public:
        enum struct ProcessType {
            client,
            service
        };

        explicit POI(QUrl &url, ProcessType type, QObject * parent = Q_NULLPTR);
        virtual ~POI();

        const ProcessType processType;

        //Verbs
        // For the service process
        Q_INVOKABLE void subscribeRequests();
        Q_INVOKABLE void unsubscribeRequests();
        Q_INVOKABLE void requestProcessed(uint32_t requestId, const QString &requestType, const QJsonValue &response);
        // For clients
        Q_INVOKABLE void sendRequest(const QString &reqestType, const QJsonValue &request);
        Q_INVOKABLE void subscribeResponses();
        Q_INVOKABLE void unsubscribeResponses();

    signals:
        void requestReceivedEvent(uint32_t requestId, const QString &requestType, const QJsonValue &response);
        void responseReceivedEvent(const QJsonValue &response);
        void connected();
        void disconnected();

    private:
        MessageEngine *m_mloop;

        // slots
        void onMessageReceived(MessageType, Message*);
        void onConnected();
        void onDisconnected();

        const QStringList events {
            POIMessage_EventNameRequestReceived,
            POIMessage_EventNameResponseReceived,
        };

        static constexpr int32_t InvalidRequestId = -1;
        int32_t requestId;
};

#endif // POI_H
