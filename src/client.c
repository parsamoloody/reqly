#include "client.h"
#include <gio/gio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    ReqlyResponseCallback callback;
    gpointer user_data;
    char *stdin_data;
} ClientRequestContext;

static const char *get_default_status_text(int code)
{
    switch (code) {
        case 200: return "OK";
        case 201: return "Created";
        case 202: return "Accepted";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 409: return "Conflict";
        case 422: return "Unprocessable Entity";
        case 429: return "Too Many Requests";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        default: return (code >= 200 && code < 300) ? "Success" : (code >= 400 ? "Error" : "");
    }
}

static void parse_curl_output(char *stdout_str, ReqlyResponse *res)
{
    if (stdout_str == NULL || *stdout_str == '\0') {
        return;
    }

    /* 1. Extract metadata from __REQLY_META__ block */
    char *meta_ptr = strstr(stdout_str, "__REQLY_META__");
    if (meta_ptr != NULL) {
        int code = 0;
        double time_sec = 0.0;
        double size_bytes = 0.0;

        if (sscanf(meta_ptr + strlen("__REQLY_META__"), "%d %lf %lf", &code, &time_sec, &size_bytes) >= 1) {
            res->status_code = code;
            res->elapsed_ms = time_sec * 1000.0;
            res->body_size = (size_t)size_bytes;
        }

        /* Cut off metadata from response payload */
        char *meta_start = strstr(stdout_str, "\n__REQLY_META__");
        if (meta_start != NULL) {
            *meta_start = '\0';
        } else {
            *meta_ptr = '\0';
        }
    }

    /* 2. Locate the final HTTP response header block */
    char *last_http = stdout_str;
    char *cursor = stdout_str;
    while ((cursor = strstr(cursor, "HTTP/")) != NULL) {
        if (cursor == stdout_str || *(cursor - 1) == '\n') {
            last_http = cursor;
        }
        cursor += 5;
    }

    /* 3. Separate headers and body */
    char *sep = strstr(last_http, "\r\n\r\n");
    size_t sep_len = 4;
    if (sep == NULL) {
        sep = strstr(last_http, "\n\n");
        sep_len = 2;
    }

    if (sep != NULL) {
        *sep = '\0';
        res->headers = g_strdup(last_http);
        res->body = g_strdup(sep + sep_len);
        if (res->body_size == 0 && res->body != NULL) {
            res->body_size = strlen(res->body);
        }
    } else {
        res->body = g_strdup(stdout_str);
        if (res->body != NULL) {
            res->body_size = strlen(res->body);
        }
    }

    /* 4. Extract reason phrase strictly from the first status line */
    char *first_line_end = strpbrk(last_http, "\r\n");
    if (first_line_end != NULL) {
        char first_line[256];
        size_t len = (size_t)(first_line_end - last_http);
        if (len >= sizeof(first_line)) {
            len = sizeof(first_line) - 1;
        }
        memcpy(first_line, last_http, len);
        first_line[len] = '\0';

        char reason[128] = {0};
        if (sscanf(first_line, "HTTP/%*s %*d %127[^\r\n]", reason) == 1) {
            g_strstrip(reason);
            if (reason[0] != '\0') {
                res->status_text = g_strdup(reason);
            }
        }
    }

    if (res->status_text == NULL) {
        res->status_text = g_strdup(get_default_status_text(res->status_code));
    }
}

static void on_communicate_done(GObject *source_object, GAsyncResult *async_res, gpointer user_data)
{
    GSubprocess *proc = G_SUBPROCESS(source_object);
    ClientRequestContext *ctx = (ClientRequestContext *)user_data;

    GBytes *stdout_bytes = NULL;
    GBytes *stderr_bytes = NULL;
    GError *error = NULL;

    g_subprocess_communicate_finish(proc, async_res, &stdout_bytes, &stderr_bytes, &error);

    ReqlyResponse *response = reqly_response_new();

    if (error != NULL) {
        response->error_message = g_strdup(error->message);
        g_error_free(error);
    } else {
        char *stdout_str = NULL;
        char *stderr_str = NULL;

        if (stdout_bytes != NULL) {
            gsize stdout_len = 0;
            const gchar *data = g_bytes_get_data(stdout_bytes, &stdout_len);
            if (data != NULL && stdout_len > 0) {
                stdout_str = g_utf8_make_valid(data, (gssize)stdout_len);
            }
        }

        if (stderr_bytes != NULL) {
            gsize stderr_len = 0;
            const gchar *data = g_bytes_get_data(stderr_bytes, &stderr_len);
            if (data != NULL && stderr_len > 0) {
                stderr_str = g_utf8_make_valid(data, (gssize)stderr_len);
            }
        }

        parse_curl_output(stdout_str, response);

        if (response->status_code == 0 && stderr_str != NULL && *stderr_str != '\0') {
            response->error_message = g_strdup(stderr_str);
        }

        g_free(stdout_str);
        g_free(stderr_str);
    }

    if (stdout_bytes != NULL) {
        g_bytes_unref(stdout_bytes);
    }
    if (stderr_bytes != NULL) {
        g_bytes_unref(stderr_bytes);
    }

    if (ctx->callback != NULL) {
        ctx->callback(response, ctx->user_data);
    }

    reqly_response_free(response);
    g_free(ctx->stdin_data);
    g_free(ctx);
    g_object_unref(proc);
}

void reqly_client_send_async(const ReqlyRequest *req,
                             ReqlyResponseCallback callback,
                             gpointer user_data)
{
    if (req == NULL || req->url == NULL || req->url[0] == '\0') {
        ReqlyResponse *res = reqly_response_new();
        res->error_message = g_strdup("URL cannot be empty.");
        if (callback != NULL) {
            callback(res, user_data);
        }
        reqly_response_free(res);
        return;
    }

    GPtrArray *argv = g_ptr_array_new_with_free_func(g_free);
    g_ptr_array_add(argv, g_strdup("curl"));
    g_ptr_array_add(argv, g_strdup("-s"));
    g_ptr_array_add(argv, g_strdup("-i"));
    g_ptr_array_add(argv, g_strdup("-L"));
    g_ptr_array_add(argv, g_strdup("--compressed"));
    g_ptr_array_add(argv, g_strdup("--max-redirs"));
    g_ptr_array_add(argv, g_strdup("10"));

    /* HTTP Method */
    g_ptr_array_add(argv, g_strdup("-X"));
    g_ptr_array_add(argv, g_strdup(reqly_http_method_to_string(req->method)));

    /* Custom Headers */
    if (req->headers != NULL && req->headers[0] != '\0') {
        gchar **lines = g_strsplit(req->headers, "\n", -1);
        for (gsize i = 0; lines[i] != NULL; i++) {
            gchar *trimmed = g_strstrip(lines[i]);
            if (trimmed[0] != '\0') {
                g_ptr_array_add(argv, g_strdup("-H"));
                g_ptr_array_add(argv, g_strdup(trimmed));
            }
        }
        g_strfreev(lines);
    }

    /* Request Body */
    gboolean has_body = (req->body != NULL && req->body[0] != '\0');
    if (has_body) {
        g_ptr_array_add(argv, g_strdup("--data-binary"));
        g_ptr_array_add(argv, g_strdup("@-"));
    }

    /* Output Timing and Status Metadata */
    g_ptr_array_add(argv, g_strdup("-w"));
    g_ptr_array_add(argv, g_strdup("\n__REQLY_META__\n%{http_code}\n%{time_total}\n%{size_download}\n"));

    /* URL */
    g_ptr_array_add(argv, g_strdup(req->url));
    g_ptr_array_add(argv, NULL);

    GSubprocessFlags flags = G_SUBPROCESS_FLAGS_STDOUT_PIPE | G_SUBPROCESS_FLAGS_STDERR_PIPE;
    if (has_body) {
        flags |= G_SUBPROCESS_FLAGS_STDIN_PIPE;
    }

    GError *error = NULL;
    GSubprocess *proc = g_subprocess_newv((const gchar * const *)argv->pdata, flags, &error);
    g_ptr_array_free(argv, TRUE);

    if (error != NULL) {
        ReqlyResponse *res = reqly_response_new();
        res->error_message = g_strdup(error->message);
        g_error_free(error);
        if (callback != NULL) {
            callback(res, user_data);
        }
        reqly_response_free(res);
        return;
    }

    ClientRequestContext *ctx = g_new0(ClientRequestContext, 1);
    ctx->callback = callback;
    ctx->user_data = user_data;
    ctx->stdin_data = has_body ? g_strdup(req->body) : NULL;

    GBytes *stdin_bytes = NULL;
    if (ctx->stdin_data != NULL) {
        stdin_bytes = g_bytes_new(ctx->stdin_data, strlen(ctx->stdin_data));
    }

    g_subprocess_communicate_async(
        proc,
        stdin_bytes,
        NULL,
        on_communicate_done,
        ctx
    );

    if (stdin_bytes != NULL) {
        g_bytes_unref(stdin_bytes);
    }
}
