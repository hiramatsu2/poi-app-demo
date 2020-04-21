#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "binding-api.h"

#include <map>
#include <mutex>
#include <string>
#include <stdexcept>

extern "C" {
// json-c/json.h
#include <json.h>
}

static std::mutex mut;
static int32_t gRequestId = 0;
static std::map<decltype(gRequestId), json_object *> gRequests;

static inline bool pushRequest(json_object *reqJson) {
    // Guard `gRequests' and `gRequestId'
    std::lock_guard<std::mutex> lock(mut);

    gRequestId++;

    json_object *reqToSvc = json_object_new_object();
    json_object_object_add(reqToSvc, POIAPI_BINDING_PARAMETER_KEY_REQUEST_ID, json_object_new_string(std::to_string(gRequestId).c_str()));

    // reqJson(1), reqToSvc(1)
    // Associate `reqJson' to `reqToSvc' but the refcount of `reqJson' is not incremented
    json_object_object_add(reqToSvc, POIAPI_BINDING_PARAMETER_KEY_REQUEST_OBJECT, reqJson);

    // reqJson(1->2->1), reqToSvc(1->2->1)
    // Each refcount of `reqJson' or `reqToSvc' is decrement, and we increment the refcounts before the decrementations
    int rc = afb_event_push(gEventRequestReceived, json_object_get(reqToSvc));
    if (rc == -1 || rc == 0) { // Discard the request if the event was not pushed or there are no subscriber
        // reqJson(1->2), reqToSvc(1->1)
        json_object_get(reqJson);
        // reqJson(2->1), reqToSvc(1->0)
        // Delete reqToSvc
        json_object_put(reqToSvc);
        return false;
    }

    // reqJson(1->2), reqToSvc(1)
    // Increment the refcount of `refJson' to preserve `reqToSvc'.
    // The refcount of reqJson will be decremented in afb_req_success
    json_object_get(reqJson);
    gRequests.emplace(gRequestId, reqToSvc);

    return true;
}

// Verb: request-processed
void send_request(afb_req_t req)
{
    json_object *reqJson = afb_req_json(req);
    if (!pushRequest(reqJson)) {
        AFB_REQ_ERROR(req, "Failed to push an event");
        // reqJson(1->0)
        afb_req_fail(req, "error", "Failed to push an event");
        return;
    }

    json_object *response = json_object_new_object();
    json_object_object_add(response, POIAPI_BINDING_PARAMETER_KEY_REQUEST_ID, json_object_new_string(std::to_string(gRequestId).c_str()));

    // `response' will be released
    // The refcount of `reqJson' will be 1; `reqJson' belonges to a element of `gRequests'
    afb_req_success(req, response, POIAPI_BINDING_VERB_SEND_REQUEST);
}

// Verb: request-processed
void request_processed(afb_req_t req)
{
    json_object *resultJson = afb_req_json(req);
    json_object *ridj;
    json_object_object_get_ex(resultJson, POIAPI_BINDING_PARAMETER_KEY_REQUEST_ID, &ridj);
    if (!json_object_is_type(ridj, json_type_string)) {
        AFB_REQ_ERROR(req, "Received response was bad formatted: %s", json_object_to_json_string_ext(afb_req_json(req), JSON_C_TO_STRING_PRETTY));
        afb_req_fail(req, "error", "Received response was badly formatted");
        return;
    }

    const char *rids = json_object_get_string(ridj);
    uint32_t rid;
    try {
        rid = std::stoi(rids);
    }
    catch (const std::invalid_argument &e) {
        AFB_REQ_ERROR(req, "Received response was bad formatted: %s", json_object_to_json_string_ext(afb_req_json(req), JSON_C_TO_STRING_PRETTY));
        afb_req_fail(req, "error", "Received response was badly formatted");
        return;
    }
    catch (const std::out_of_range &e) {
        AFB_REQ_ERROR(req, "Received response was bad formatted: %s", json_object_to_json_string_ext(afb_req_json(req), JSON_C_TO_STRING_PRETTY));
        afb_req_fail(req, "error", "Received response was badly formatted");
        return;
    }

    decltype(gRequests)::iterator it = gRequests.find(rid);
    if (it == gRequests.end()) {
        AFB_REQ_ERROR(req, "Received an invalid request id: %d", rid);
        afb_req_fail_f(req, "error", "Received an invalid request id: %d %s", rid, rids);
        return;
    }

    // Preserve `resultJson' because afb_req_success use it
    if (afb_event_push(gEventResponseReceived, json_object_get(resultJson)) == -1) {
        AFB_REQ_ERROR(req, "Failed to push an event");
        afb_req_fail(req, "error", "Failed to push an event");
        return;
    }

    afb_req_success(req, nullptr, nullptr);

    // Guard `gRequests'
    std::lock_guard<std::mutex> lock(mut);

    // Delete the processed request data
    json_object *processedReq = it->second;
    gRequests.erase(it);
    int rc = json_object_put(processedReq);
    if (rc != 1) {
        AFB_REQ_ERROR(req, "bad-ref-count %d:%s", rc, POIAPI_BINDING_VERB_REQUEST_PROCESSED);
    }
}

// Verb: subscribe-requests
void subscribe_requests(afb_req_t req)
{
    if (afb_req_subscribe(req, gEventRequestReceived) == 0) {
        afb_req_success(req, nullptr, nullptr);
    } else {
        AFB_REQ_ERROR(req, "Failed to %s", POIAPI_BINDING_VERB_SUBSCRIBE_REQUESTS);
        afb_req_fail_f(req, "error", "Failed to %s", POIAPI_BINDING_VERB_SUBSCRIBE_REQUESTS);
    }
}

// Verb: unsubscribe-requests
void unsubscribe_requests(afb_req_t req)
{
    if (afb_req_unsubscribe(req, gEventRequestReceived) == 0) {
        afb_req_success(req, nullptr, nullptr);
    } else {
        AFB_REQ_ERROR(req, "Failed to %s", POIAPI_BINDING_VERB_UNSUBSCRIBE_REQUESTS);
        afb_req_fail_f(req, "error", "Failed to %s", POIAPI_BINDING_VERB_UNSUBSCRIBE_REQUESTS);
    }
}

// Verb: subscribe-responses
void subscribe_responses(afb_req_t req)
{
    if (afb_req_subscribe(req, gEventResponseReceived) == 0) {
        afb_req_success(req, nullptr, nullptr);
    } else {
        AFB_REQ_ERROR(req, "Failed to %s", POIAPI_BINDING_VERB_SUBSCRIBE_RESPONSES);
        afb_req_fail_f(req, "error", "Failed to %s", POIAPI_BINDING_VERB_SUBSCRIBE_RESPONSES);
    }
}

// Verb: unsubscribe-responses
void unsubscribe_responses(afb_req_t req)
{
    if (afb_req_unsubscribe(req, gEventResponseReceived) == 0) {
        afb_req_success(req, nullptr, nullptr);
    } else {
        AFB_REQ_ERROR(req, "Failed to %s", POIAPI_BINDING_VERB_UNSUBSCRIBE_RESPONSES);
        afb_req_fail_f(req, "error", "Failed to %s", POIAPI_BINDING_VERB_UNSUBSCRIBE_RESPONSES);
    }
}
