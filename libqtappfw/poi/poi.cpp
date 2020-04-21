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

#include "message.h"
#include "messageengine.h"
#include "poi.h"
#include "poimessage.h"
#include "responsemessage.h"

#include <QJsonDocument>

POI::POI (QUrl &url, ProcessType type, QObject * parent) :
    QObject(parent),
    processType(type),
    m_mloop(nullptr),
    requestId(InvalidRequestId)
{
    m_mloop = new MessageEngine(url);
    QObject::connect(m_mloop, &MessageEngine::connected, this, &POI::onConnected);
    QObject::connect(m_mloop, &MessageEngine::disconnected, this, &POI::onDisconnected);
    QObject::connect(m_mloop, &MessageEngine::messageReceived, this, &POI::onMessageReceived);
}

POI::~POI()
{
    delete m_mloop;
}

void POI::subscribeRequests()
{
    POIMessage *nmsg = new POIMessage();
    nmsg->createRequest(POIMessage_VerbSubscribeRequests, QJsonObject());
    m_mloop->sendMessage(nmsg);
    delete nmsg;
}

void POI::unsubscribeRequests()
{
    POIMessage *nmsg = new POIMessage();
    nmsg->createRequest(POIMessage_VerbUnsubscribeRequests, QJsonObject());
    m_mloop->sendMessage(nmsg);
    delete nmsg;
}

void POI::requestProcessed(uint32_t requestId, const QString &requestType, const QJsonValue &response)
{
    POIMessage *nmsg = new POIMessage();
    QJsonObject parameter;
    parameter.insert(POI_ParameterKeyRequestId, QString::number(requestId));
    parameter.insert(POI_ParameterKeyRequestType, requestType);
    parameter.insert(POI_ParameterKeyResponseObject, response);
    nmsg->createRequest(POIMessage_VerbRequestProcessed, parameter);
    m_mloop->sendMessage(nmsg);
    delete nmsg;
}

void POI::subscribeResponses()
{
    POIMessage *nmsg = new POIMessage();
    nmsg->createRequest(POIMessage_VerbSubscribeResponses, QJsonObject());
    m_mloop->sendMessage(nmsg);
    delete nmsg;
}

void POI::unsubscribeResponses()
{
    POIMessage *nmsg = new POIMessage();
    nmsg->createRequest(POIMessage_VerbUnsubscribeResponses, QJsonObject());
    m_mloop->sendMessage(nmsg);
    delete nmsg;
}

void POI::sendRequest(const QString &requestType, const QJsonValue &request)
{
    POIMessage *nmsg = new POIMessage();
    QJsonObject parameter;
    parameter.insert(POI_ParameterKeyRequestType, requestType);
    parameter.insert(POI_ParameterKeyRequestParameter, request);
    nmsg->createRequest(POIMessage_VerbSendRequest, parameter);
    m_mloop->sendMessage(nmsg);
    delete nmsg;
}

void POI::onConnected()
{
    QStringListIterator eventIterator(events);
    POIMessage *nmsg;

    while (eventIterator.hasNext()) {
        QJsonObject parameter;
        parameter.insert("value", eventIterator.next());

        switch (processType) {
        case ProcessType::client:
            subscribeResponses();
            break;
        case ProcessType::service:
            subscribeRequests();
            break;
        }
    }

    emit connected();
}

void POI::onDisconnected()
{
    QStringListIterator eventIterator(events);
    POIMessage *nmsg;

    while (eventIterator.hasNext()) {
        QJsonObject parameter;
        parameter.insert("value", eventIterator.next());

        switch (processType) {
        case ProcessType::client:
            unsubscribeResponses();
            break;
        case ProcessType::service:
            unsubscribeRequests();
            break;
        };
    }

    emit disconnected();
}

void POI::onMessageReceived(MessageType type, Message *msg)
{
    if (type == POIEventMessage) {
        POIMessage *tmsg = qobject_cast<POIMessage*>(msg);

        if (tmsg->isRequestReceivedEvent()) {
            // For POI service
            QJsonValue rid = tmsg->eventData()[POI_ParameterKeyRequestId];
            QJsonValue rv = tmsg->eventData()[POI_ParameterKeyRequestObject];
            if ((!rid.isUndefined()) && rid.isString() &&
                (!rv.isUndefined()) && rv.isObject()) {
                QJsonObject robj = rv.toObject();
                bool ok;
                int32_t reqid = rid.toString().toInt(&ok);
                if (ok) {
                    emit requestReceivedEvent(reqid, robj[POI_ParameterKeyRequestType].toString(), robj[POI_ParameterKeyRequestParameter].toObject());
                }
            }
        }
        if (tmsg->isResponseReceivedEvent()) {
            // For POI client
            QJsonValue rid = tmsg->eventData()[POI_ParameterKeyRequestId];
            if ((!rid.isUndefined()) && rid.isString()) {
                bool ok;
                int32_t reqid = rid.toString().toInt(&ok);
                if (ok && (reqid == requestId)) {
                    emit responseReceivedEvent(tmsg->eventData()[POI_ParameterKeyResponseObject]);
                    requestId = InvalidRequestId;
                }
            }
        }
    }
    else
    {
        ResponseMessage *tmsg = qobject_cast<ResponseMessage*>(msg);

        if (tmsg->isReply()) {
            // For POI client
            QJsonValue rid = tmsg->replyData()[POI_ParameterKeyRequestId];
            if ((!rid.isUndefined()) && rid.isString()) {
                bool ok;
                int32_t reqid = rid.toString().toInt(&ok);
                if (ok) {
                    requestId = reqid;
                }
                else {
                    requestId = InvalidRequestId;
                }
            }
        }
    }

    msg->deleteLater();
}
