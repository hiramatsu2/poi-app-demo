#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "binding-api.h"

#include <stdlib.h>
#include <stdio.h>

const afb_verb_t verbs[] = {
    { .verb = POIAPI_BINDING_VERB_SEND_REQUEST, .callback = send_request },
    { .verb = POIAPI_BINDING_VERB_REQUEST_PROCESSED, .callback = request_processed },
    { .verb = POIAPI_BINDING_VERB_SUBSCRIBE_REQUESTS, .callback = subscribe_requests },
    { .verb = POIAPI_BINDING_VERB_UNSUBSCRIBE_REQUESTS, .callback = unsubscribe_requests },
    { .verb = POIAPI_BINDING_VERB_SUBSCRIBE_RESPONSES, .callback = subscribe_responses },
    { .verb = POIAPI_BINDING_VERB_UNSUBSCRIBE_RESPONSES, .callback = unsubscribe_responses },
    { .verb = NULL }
};

afb_event_t gEventRequestReceived;
afb_event_t gEventResponseReceived;

int init(afb_api_t api)
{
    gEventRequestReceived = afb_api_make_event(api, POIAPI_BINDING_EVENT_REQUEST_RECEIVED);
    gEventResponseReceived = afb_api_make_event(api, POIAPI_BINDING_EVENT_RESPONSE_RECEIVED);
    if (afb_event_is_valid(gEventRequestReceived) && afb_event_is_valid(gEventResponseReceived)) {

        json_object *args = json_object_new_object();
        // TODO Set Version
        json_object *appid = json_object_new_string("poi-service@0.1");
        json_object_object_add(args, "id", appid);

        char *err;
        char *info;
        json_object *response;

        // Launch poiservice
        afb_api_call_sync(api, "afm-main", "start", args, &response, &err, &info);

        if (err) {
            AFB_API_ERROR(api, "ERROR %s", err);
            free(err);
        }
        if (info) {
            AFB_API_INFO(api, "INFO %s", info);
            free(info);
        }

        json_object_put(response);

        return 0;
    }
    else {
        AFB_API_ERROR(api, "Can't create events");
        return -1;
    }
}

const afb_binding_t afbBindingExport = {
    .api = POIAPI_BINDING_API_NAME,
    .verbs = verbs,
    .init = init,
    .noconcurrency = 0
};
