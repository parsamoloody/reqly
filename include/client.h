#ifndef REQLY_CLIENT_H
#define REQLY_CLIENT_H

#include "request.h"
#include <glib.h>

/**
 * Callback invoked on the GTK main loop when an async request finishes.
 *
 * @param response The completed response.
 * @param user_data Caller-provided context data.
 */
typedef void (*ReqlyResponseCallback)(const ReqlyResponse *response, gpointer user_data);

/**
 * Executes an HTTP request asynchronously without blocking the main event loop.
 *
 * @param req The request to execute.
 * @param callback Completion callback.
 * @param user_data Data passed through to callback.
 */
void reqly_client_send_async(const ReqlyRequest *req,
                             ReqlyResponseCallback callback,
                             gpointer user_data);

#endif /* REQLY_CLIENT_H */
