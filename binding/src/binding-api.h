#pragma once

#ifndef AFB_BINDING_VERSION
#define AFB_BINDING_VERSION 3
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <afb/afb-binding.h>

extern afb_event_t gEventRequestReceived;
extern afb_event_t gEventResponseReceived;

// Verbs
extern void send_request(afb_req_t req);
extern void request_processed(afb_req_t req);
extern void subscribe_requests(afb_req_t req);
extern void unsubscribe_requests(afb_req_t req);
extern void subscribe_responses(afb_req_t req);
extern void unsubscribe_responses(afb_req_t req);

#define POIAPI_BINDING_API_NAME "poi"

#define POIAPI_BINDING_VERB_SEND_REQUEST "send-request"
#define POIAPI_BINDING_VERB_REQUEST_PROCESSED "request-processed"
#define POIAPI_BINDING_VERB_SUBSCRIBE_REQUESTS "subscribe-requests"
#define POIAPI_BINDING_VERB_UNSUBSCRIBE_REQUESTS "unsubscribe-requests"
#define POIAPI_BINDING_VERB_SUBSCRIBE_RESPONSES "subscribe-responses"
#define POIAPI_BINDING_VERB_UNSUBSCRIBE_RESPONSES "unsubscribe-responses"

#define POIAPI_BINDING_EVENT_REQUEST_RECEIVED "reqest-received-event"
#define POIAPI_BINDING_EVENT_RESPONSE_RECEIVED "response-received-event"

// Some definitions is not used in this project

#define POIAPI_BINDING_PARAMETER_KEY_REQUEST_ID "request-id"
#define POIAPI_BINDING_PARAMETER_KEY_REQUEST_OBJECT "request-object"
#define POIAPI_BINDING_PARAMETER_KEY_RESPONSE_OBJECT "response-object"

#define POIAPI_BINDING_REQUEST_OBJECT_KEY_REQUEST_TYPE "request-type"
#define POIAPI_BINDING_REQUEST_OBJECT_KEY_REQUEST_PARAMETER "request-parameter"

#define POIAPI_BINDING_REQUEST_TYPE_EXTERNAL_ONLINE_REQUEST_HTTP_GET_JSON "external-online-request-http-get-json"

#ifdef __cplusplus
} // extern "C"
#endif
