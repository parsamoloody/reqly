#include "protocol.h"
#include "http.h"
#include <string.h>
#include <strings.h>

static const char * const protocol_names[] = {
    "HTTP",
    NULL
};

static const ReqlyProtocol protocol_registry[] = {
    {
        .id = REQLY_PROTOCOL_HTTP,
        .name = "HTTP",
        .default_scheme = "https://"
    }
};

size_t reqly_protocol_get_count(void)
{
    return REQLY_PROTOCOL_COUNT;
}

const ReqlyProtocol *reqly_protocol_get_by_id(ReqlyProtocolId id)
{
    if ((size_t)id < REQLY_PROTOCOL_COUNT) {
        return &protocol_registry[id];
    }
    return NULL;
}

const ReqlyProtocol *reqly_protocol_get_by_index(size_t index)
{
    if (index < REQLY_PROTOCOL_COUNT) {
        return &protocol_registry[index];
    }
    return NULL;
}

const ReqlyProtocol *reqly_protocol_find_by_name(const char *name)
{
    if (name == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < REQLY_PROTOCOL_COUNT; i++) {
        if (strcasecmp(protocol_registry[i].name, name) == 0) {
            return &protocol_registry[i];
        }
    }
    return NULL;
}

const ReqlyProtocol *reqly_protocol_find_by_url(const char *url)
{
    if (url == NULL) {
        return NULL;
    }

    if (strncasecmp(url, "http://", 7) == 0 || strncasecmp(url, "https://", 8) == 0) {
        return &protocol_registry[REQLY_PROTOCOL_HTTP];
    }

    return NULL;
}

const char * const *reqly_protocol_get_names(void)
{
    return protocol_names;
}

const char * const *reqly_protocol_get_methods(ReqlyProtocolId id, size_t *out_count)
{
    if (id == REQLY_PROTOCOL_HTTP) {
        if (out_count != NULL) {
            *out_count = reqly_http_get_method_count();
        }
        return reqly_http_get_method_names();
    }

    if (out_count != NULL) {
        *out_count = 0;
    }
    return NULL;
}

