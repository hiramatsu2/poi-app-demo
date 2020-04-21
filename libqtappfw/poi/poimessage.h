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

#ifndef POI_MESSAGE_H
#define POI_MESSAGE_H

#include "message.h"

// Events for clients
static constexpr char POIMessage_EventNameResponseReceived[] = "response-received-event";

// Events for service
static constexpr char POIMessage_EventNameRequestReceived[] = "reqest-received-event";

// Verbs for clients
static constexpr char POIMessage_VerbSubscribeResponses[] = "subscribe-responses";
static constexpr char POIMessage_VerbUnsubscribeResponses[] = "unsubscribe-responses";
static constexpr char POIMessage_VerbSendRequest[] = "send-request";

// Verbs for services
static constexpr char POIMessage_VerbSubscribeRequests[] = "subscribe-requests";
static constexpr char POIMessage_VerbUnsubscribeRequests[] = "unsubscribe-requests";
static constexpr char POIMessage_VerbRequestProcessed[] = "request-processed";

class POIMessage : public Message
{
    Q_OBJECT
    public:

        bool createRequest(QString verb, QJsonObject parameter);
        bool isRequestReceivedEvent() { return (this->eventName() == POIMessage_EventNameRequestReceived); };
        bool isResponseReceivedEvent() { return (this->eventName() == POIMessage_EventNameResponseReceived); };

    private:
        QStringList verbs {
            POIMessage_VerbSubscribeResponses,
            POIMessage_VerbUnsubscribeResponses,
            POIMessage_VerbSendRequest,
            POIMessage_VerbSubscribeRequests,
            POIMessage_VerbUnsubscribeRequests,
            POIMessage_VerbRequestProcessed,
        };
};

#endif // POI_MESSAGE_H
