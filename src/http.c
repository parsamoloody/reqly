#include "http.h"
#include <string.h>

static const char * const http_method_names[] = {
    "GET",
    "POST",
    "PUT",
    "DELETE",
    "PATCH",
    "HEAD",
    "OPTIONS",
    NULL
};

const char *reqly_http_method_to_string(ReqlyHttpMethod method)
{
    if ((size_t)method < REQLY_HTTP_METHOD_COUNT) {
        return http_method_names[method];
    }
    return "GET";
}

ReqlyHttpMethod reqly_http_method_from_string(const char *method_str)
{
    if (method_str == NULL) {
        return REQLY_HTTP_METHOD_GET;
    }

    for (size_t i = 0; i < REQLY_HTTP_METHOD_COUNT; i++) {
        if (strcmp(http_method_names[i], method_str) == 0) {
            return (ReqlyHttpMethod)i;
        }
    }

    return REQLY_HTTP_METHOD_GET;
}

const char * const *reqly_http_get_method_names(void)
{
    return http_method_names;
}

size_t reqly_http_get_method_count(void)
{
    return REQLY_HTTP_METHOD_COUNT;
}

