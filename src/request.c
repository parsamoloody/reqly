#include "request.h"
#include <glib.h>

ReqlyRequest *reqly_request_new(void)
{
    ReqlyRequest *req = g_new0(ReqlyRequest, 1);
    req->protocol = REQLY_PROTOCOL_HTTP;
    req->method = REQLY_HTTP_METHOD_GET;
    return req;
}

void reqly_request_free(ReqlyRequest *req)
{
    if (req == NULL) {
        return;
    }

    g_free(req->url);
    g_free(req->headers);
    g_free(req->body);
    g_free(req);
}

ReqlyResponse *reqly_response_new(void)
{
    return g_new0(ReqlyResponse, 1);
}

void reqly_response_free(ReqlyResponse *res)
{
    if (res == NULL) {
        return;
    }

    g_free(res->status_text);
    g_free(res->headers);
    g_free(res->body);
    g_free(res->error_message);
    g_free(res);
}
