#include "ui.h"
#include "protocol.h"
#include "http.h"
#include "request.h"
#include "client.h"
#include <stdio.h>

typedef struct {
    GtkWidget *protocol_dropdown;
    GtkWidget *method_dropdown;
    GtkWidget *url_entry;
    GtkWidget *send_button;

    GtkWidget *request_headers_view;
    GtkWidget *request_body_view;

    GtkWidget *status_badge;
    GtkWidget *time_label;
    GtkWidget *size_label;

    GtkWidget *response_body_view;
    GtkWidget *response_headers_view;
} ReqlyUIContext;

static char *get_text_view_string(GtkWidget *text_view)
{
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(buf, &start);
    gtk_text_buffer_get_end_iter(buf, &end);
    return gtk_text_buffer_get_text(buf, &start, &end, FALSE);
}

static void set_text_view_string(GtkWidget *text_view, const char *text)
{
    GtkTextBuffer *buf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_buffer_set_text(buf, text ? text : "", -1);
}

static void on_response_received(const ReqlyResponse *res, gpointer user_data)
{
    ReqlyUIContext *ctx = (ReqlyUIContext *)user_data;

    /* Re-enable Send button */
    gtk_widget_set_sensitive(ctx->send_button, TRUE);
    gtk_button_set_label(GTK_BUTTON(ctx->send_button), "Send");

    if (res == NULL) {
        gtk_label_set_text(GTK_LABEL(ctx->status_badge), "Status: Error");
        set_text_view_string(ctx->response_body_view, "No response received.");
        return;
    }

    /* Error handling */
    if (res->error_message != NULL && res->status_code == 0) {
        gtk_label_set_text(GTK_LABEL(ctx->status_badge), "Status: Connection Error");
        gtk_label_set_text(GTK_LABEL(ctx->time_label), "Time: --");
        gtk_label_set_text(GTK_LABEL(ctx->size_label), "Size: --");

        char err_msg[1024];
        snprintf(err_msg, sizeof(err_msg), "Error executing request:\n%s", res->error_message);
        set_text_view_string(ctx->response_body_view, err_msg);
        set_text_view_string(ctx->response_headers_view, "");
        return;
    }

    /* Status Badge */
    char status_str[128];
    snprintf(status_str, sizeof(status_str), "Status: %d %s",
             res->status_code,
             res->status_text ? res->status_text : "");
    gtk_label_set_text(GTK_LABEL(ctx->status_badge), status_str);

    /* Time Metrics */
    char time_str[64];
    if (res->elapsed_ms >= 1000.0) {
        snprintf(time_str, sizeof(time_str), "Time: %.2f s", res->elapsed_ms / 1000.0);
    } else {
        snprintf(time_str, sizeof(time_str), "Time: %.0f ms", res->elapsed_ms);
    }
    gtk_label_set_text(GTK_LABEL(ctx->time_label), time_str);

    /* Size Metrics */
    char size_str[64];
    if (res->body_size >= 1048576) {
        snprintf(size_str, sizeof(size_str), "Size: %.2f MB", (double)res->body_size / 1048576.0);
    } else if (res->body_size >= 1024) {
        snprintf(size_str, sizeof(size_str), "Size: %.2f KB", (double)res->body_size / 1024.0);
    } else {
        snprintf(size_str, sizeof(size_str), "Size: %zu B", res->body_size);
    }
    gtk_label_set_text(GTK_LABEL(ctx->size_label), size_str);

    /* Response Body & Headers */
    set_text_view_string(ctx->response_body_view, res->body ? res->body : "");
    set_text_view_string(ctx->response_headers_view, res->headers ? res->headers : "");
}

static void on_send_requested(GtkWidget *widget, gpointer user_data)
{
    (void)widget;
    ReqlyUIContext *ctx = (ReqlyUIContext *)user_data;

    const char *url = gtk_editable_get_text(GTK_EDITABLE(ctx->url_entry));
    if (url == NULL || url[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(ctx->status_badge), "Status: Please enter a URL");
        return;
    }

    guint proto_idx = gtk_drop_down_get_selected(GTK_DROP_DOWN(ctx->protocol_dropdown));
    guint method_idx = gtk_drop_down_get_selected(GTK_DROP_DOWN(ctx->method_dropdown));

    /* UI feedback: sending */
    gtk_widget_set_sensitive(ctx->send_button, FALSE);
    gtk_button_set_label(GTK_BUTTON(ctx->send_button), "Sending...");
    gtk_label_set_text(GTK_LABEL(ctx->status_badge), "Status: Sending...");
    gtk_label_set_text(GTK_LABEL(ctx->time_label), "Time: --");
    gtk_label_set_text(GTK_LABEL(ctx->size_label), "Size: --");

    ReqlyRequest *req = reqly_request_new();
    req->protocol = (ReqlyProtocolId)proto_idx;
    req->method = (ReqlyHttpMethod)method_idx;
    req->url = g_strdup(url);
    req->headers = get_text_view_string(ctx->request_headers_view);
    req->body = get_text_view_string(ctx->request_body_view);

    reqly_client_send_async(req, on_response_received, ctx);
    reqly_request_free(req);
}

static void on_protocol_changed(GObject *gobject, GParamSpec *pspec, gpointer user_data)
{
    (void)pspec;
    ReqlyUIContext *ctx = (ReqlyUIContext *)user_data;
    guint proto_idx = gtk_drop_down_get_selected(GTK_DROP_DOWN(gobject));
    const char * const *methods = reqly_protocol_get_methods((ReqlyProtocolId)proto_idx, NULL);

    if (methods != NULL) {
        GtkStringList *string_list = gtk_string_list_new(methods);
        gtk_drop_down_set_model(GTK_DROP_DOWN(ctx->method_dropdown), G_LIST_MODEL(string_list));
        g_object_unref(string_list);
    }
}

static GtkWidget *create_scrollable_text_view(GtkWidget **out_view, gboolean editable, gboolean monospace)
{
    GtkWidget *view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(view), editable);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(view), GTK_WRAP_WORD_CHAR);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(view), 8);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(view), 8);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(view), 8);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(view), 8);

    if (monospace) {
        gtk_widget_add_css_class(view, "monospace");
    }

    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), view);
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_widget_set_hexpand(scrolled, TRUE);

    if (out_view != NULL) {
        *out_view = view;
    }
    return scrolled;
}

GtkWidget *reqly_ui_create_main_window(GtkApplication *app)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Reqly");
    gtk_window_set_default_size(GTK_WINDOW(window), 960, 640);

    /* Header Bar */
    GtkWidget *header_bar = gtk_header_bar_new();
    GtkWidget *title_widget = gtk_label_new("Reqly");
    gtk_widget_add_css_class(title_widget, "title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header_bar), title_widget);
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    /* UI Context */
    ReqlyUIContext *ctx = g_new0(ReqlyUIContext, 1);
    g_object_set_data_full(G_OBJECT(window), "reqly_ui_ctx", ctx, g_free);

    /* Root Layout Box */
    GtkWidget *root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* --- Request Bar --- */
    GtkWidget *request_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_start(request_bar, 12);
    gtk_widget_set_margin_end(request_bar, 12);
    gtk_widget_set_margin_top(request_bar, 10);
    gtk_widget_set_margin_bottom(request_bar, 10);

    ctx->protocol_dropdown = gtk_drop_down_new_from_strings(reqly_protocol_get_names());
    gtk_widget_set_tooltip_text(ctx->protocol_dropdown, "Select Protocol");
    gtk_box_append(GTK_BOX(request_bar), ctx->protocol_dropdown);

    ctx->method_dropdown = gtk_drop_down_new_from_strings(reqly_http_get_method_names());
    gtk_widget_set_tooltip_text(ctx->method_dropdown, "Select HTTP Method");
    gtk_box_append(GTK_BOX(request_bar), ctx->method_dropdown);

    g_signal_connect(ctx->protocol_dropdown, "notify::selected", G_CALLBACK(on_protocol_changed), ctx);

    ctx->url_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->url_entry), "https://httpbin.org/get");
    gtk_widget_set_hexpand(ctx->url_entry, TRUE);
    g_signal_connect(ctx->url_entry, "activate", G_CALLBACK(on_send_requested), ctx);
    gtk_box_append(GTK_BOX(request_bar), ctx->url_entry);

    ctx->send_button = gtk_button_new_with_label("Send");
    gtk_widget_add_css_class(ctx->send_button, "suggested-action");
    g_signal_connect(ctx->send_button, "clicked", G_CALLBACK(on_send_requested), ctx);
    gtk_box_append(GTK_BOX(request_bar), ctx->send_button);

    gtk_box_append(GTK_BOX(root_box), request_bar);

    /* Separator */
    gtk_box_append(GTK_BOX(root_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL));

    /* --- Paned Split View (Request / Response) --- */
    GtkWidget *paned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_paned_set_position(GTK_PANED(paned), 200);
    gtk_paned_set_resize_start_child(GTK_PANED(paned), FALSE);
    gtk_paned_set_resize_end_child(GTK_PANED(paned), TRUE);
    gtk_widget_set_vexpand(paned, TRUE);

    /* 1. Request Notebook (Top Pane) */
    GtkWidget *req_notebook = gtk_notebook_new();
    gtk_widget_set_margin_start(req_notebook, 8);
    gtk_widget_set_margin_end(req_notebook, 8);
    gtk_widget_set_margin_top(req_notebook, 6);

    GtkWidget *headers_scrolled = create_scrollable_text_view(&ctx->request_headers_view, TRUE, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(req_notebook), headers_scrolled, gtk_label_new("Headers"));

    GtkWidget *body_scrolled = create_scrollable_text_view(&ctx->request_body_view, TRUE, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(req_notebook), body_scrolled, gtk_label_new("Body"));

    gtk_paned_set_start_child(GTK_PANED(paned), req_notebook);

    /* 2. Response Area (Bottom Pane) */
    GtkWidget *res_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_set_margin_start(res_box, 8);
    gtk_widget_set_margin_end(res_box, 8);
    gtk_widget_set_margin_top(res_box, 6);
    gtk_widget_set_margin_bottom(res_box, 8);

    /* Response Status Bar */
    GtkWidget *res_status_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16);
    gtk_widget_set_margin_start(res_status_bar, 4);
    gtk_widget_set_margin_bottom(res_status_bar, 4);

    ctx->status_badge = gtk_label_new("Status: --");
    gtk_widget_add_css_class(ctx->status_badge, "heading");
    gtk_box_append(GTK_BOX(res_status_bar), ctx->status_badge);

    ctx->time_label = gtk_label_new("Time: --");
    gtk_widget_add_css_class(ctx->time_label, "dim-label");
    gtk_box_append(GTK_BOX(res_status_bar), ctx->time_label);

    ctx->size_label = gtk_label_new("Size: --");
    gtk_widget_add_css_class(ctx->size_label, "dim-label");
    gtk_box_append(GTK_BOX(res_status_bar), ctx->size_label);

    gtk_box_append(GTK_BOX(res_box), res_status_bar);

    /* Response Notebook */
    GtkWidget *res_notebook = gtk_notebook_new();
    gtk_widget_set_vexpand(res_notebook, TRUE);

    GtkWidget *res_body_scrolled = create_scrollable_text_view(&ctx->response_body_view, FALSE, TRUE);
    set_text_view_string(ctx->response_body_view, "Response body will appear here...");
    gtk_notebook_append_page(GTK_NOTEBOOK(res_notebook), res_body_scrolled, gtk_label_new("Response Body"));

    GtkWidget *res_headers_scrolled = create_scrollable_text_view(&ctx->response_headers_view, FALSE, TRUE);
    gtk_notebook_append_page(GTK_NOTEBOOK(res_notebook), res_headers_scrolled, gtk_label_new("Response Headers"));

    gtk_box_append(GTK_BOX(res_box), res_notebook);

    gtk_paned_set_end_child(GTK_PANED(paned), res_box);

    gtk_box_append(GTK_BOX(root_box), paned);

    gtk_window_set_child(GTK_WINDOW(window), root_box);
    gtk_window_present(GTK_WINDOW(window));

    return window;
}
