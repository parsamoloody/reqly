#ifndef REQLY_HTTP_H
#define REQLY_HTTP_H

#include <stddef.h>

/**
 * Standard HTTP request methods.
 */
typedef enum {
    REQLY_HTTP_METHOD_GET = 0,
    REQLY_HTTP_METHOD_POST,
    REQLY_HTTP_METHOD_PUT,
    REQLY_HTTP_METHOD_DELETE,
    REQLY_HTTP_METHOD_PATCH,
    REQLY_HTTP_METHOD_HEAD,
    REQLY_HTTP_METHOD_OPTIONS,
    REQLY_HTTP_METHOD_COUNT
} ReqlyHttpMethod;

/**
 * Returns string representation of an HTTP method (e.g. "GET").
 */
const char *reqly_http_method_to_string(ReqlyHttpMethod method);

/**
 * Parses an HTTP method from string. Returns REQLY_HTTP_METHOD_GET if unrecognized.
 */
ReqlyHttpMethod reqly_http_method_from_string(const char *method_str);

/**
 * Returns a NULL-terminated array of method names.
 */
const char * const *reqly_http_get_method_names(void);

/**
 * Returns the number of supported HTTP methods.
 */
size_t reqly_http_get_method_count(void);

#endif /* REQLY_HTTP_H */
