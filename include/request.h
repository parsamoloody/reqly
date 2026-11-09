#ifndef REQLY_REQUEST_H
#define REQLY_REQUEST_H

#include "protocol.h"
#include "http.h"
#include <stddef.h>

/**
 * Represents an outgoing API request.
 */
typedef struct {
    ReqlyProtocolId protocol;
    ReqlyHttpMethod method;
    char *url;
    char *headers;  /* Multi-line headers (Name: Value\n...) */
    char *body;     /* Optional payload */
} ReqlyRequest;

/**
 * Represents the response received from an API request.
 */
typedef struct {
    int status_code;
    char *status_text;
    char *headers;
    char *body;
    size_t body_size;
    double elapsed_ms;
    char *error_message;
} ReqlyResponse;

/**
 * Allocates and initializes a new empty request.
 */
ReqlyRequest *reqly_request_new(void);

/**
 * Frees a request and all its allocated fields.
 */
void reqly_request_free(ReqlyRequest *req);

/**
 * Allocates and initializes a new empty response.
 */
ReqlyResponse *reqly_response_new(void);

/**
 * Frees a response and all its allocated fields.
 */
void reqly_response_free(ReqlyResponse *res);

#endif /* REQLY_REQUEST_H */
