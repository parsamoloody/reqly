#include "ui.h"
#include "protocol.h"
#include "http.h"
#include <stdio.h>

typedef struct {
    GtkWidget *protocol_dropdown;
    GtkWidget *method_dropdown;
    GtkWidget *url_entry;
    GtkWidget *status_label;
} ReqlyUIContext;

static void on_send_requested(GtkWidget *widget, gpointer user_data)
{
    (void)widget;
    ReqlyUIContext *ctx = (ReqlyUIContext *)user_data;

    const char *url = gtk_editable_get_text(GTK_EDITABLE(ctx->url_entry));
    guint proto_idx = gtk_drop_down_get_selected(GTK_DROP_DOWN(ctx->protocol_dropdown));
    guint method_idx = gtk_drop_down_get_selected(GTK_DROP_DOWN(ctx->method_dropdown));

    const ReqlyProtocol *proto = reqly_protocol_get_by_index(proto_idx);
    const char *proto_name = (proto != NULL) ? proto->name : "HTTP";

    const char * const *methods = reqly_protocol_get_methods((ReqlyProtocolId)proto_idx, NULL);
    const char *method_name = "GET";
    if (methods != NULL) {
        for (guint i = 0; methods[i] != NULL; i++) {
            if (i == method_idx) {
                method_name = methods[i];
                break;
            }
        }
    }

    if (url == NULL || url[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(ctx->status_label), "Please enter a URL to send a request.");
        return;
    }

    char message[512];
    snprintf(message, sizeof(message), "Ready to send: [%s] %s %s", proto_name, method_name, url);
    gtk_label_set_text(GTK_LABEL(ctx->status_label), message);
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

GtkWidget *reqly_ui_create_main_window(GtkApplication *app)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Reqly");
    gtk_window_set_default_size(GTK_WINDOW(window), 850, 500);

    /* Header Bar */
    GtkWidget *header_bar = gtk_header_bar_new();
    GtkWidget *title_widget = gtk_label_new("Reqly");
    gtk_widget_add_css_class(title_widget, "title");
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header_bar), title_widget);
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    /* Allocate UI Context (owned by the window) */
    ReqlyUIContext *ctx = g_new0(ReqlyUIContext, 1);
    g_object_set_data_full(G_OBJECT(window), "reqly_ui_ctx", ctx, g_free);

    /* Root Container */
    GtkWidget *root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    /* Request Bar Container */
    GtkWidget *request_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);
    gtk_widget_set_margin_start(request_bar, 16);
    gtk_widget_set_margin_end(request_bar, 16);
    gtk_widget_set_margin_top(request_bar, 16);
    gtk_widget_set_margin_bottom(request_bar, 16);

    /* 1. Protocol Dropdown */
    ctx->protocol_dropdown = gtk_drop_down_new_from_strings(reqly_protocol_get_names());
    gtk_widget_set_tooltip_text(ctx->protocol_dropdown, "Select Protocol");
    gtk_box_append(GTK_BOX(request_bar), ctx->protocol_dropdown);

    /* 2. Method Dropdown */
    ctx->method_dropdown = gtk_drop_down_new_from_strings(reqly_http_get_method_names());
    gtk_widget_set_tooltip_text(ctx->method_dropdown, "Select HTTP Method");
    gtk_box_append(GTK_BOX(request_bar), ctx->method_dropdown);

    /* Listen for protocol changes to update methods dynamically */
    g_signal_connect(ctx->protocol_dropdown, "notify::selected", G_CALLBACK(on_protocol_changed), ctx);

    /* 3. URL Entry */
    ctx->url_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(ctx->url_entry), "https://api.example.com/v1/resource");
    gtk_widget_set_hexpand(ctx->url_entry, TRUE);
    g_signal_connect(ctx->url_entry, "activate", G_CALLBACK(on_send_requested), ctx);
    gtk_box_append(GTK_BOX(request_bar), ctx->url_entry);

    /* 4. Send Button */
    GtkWidget *send_button = gtk_button_new_with_label("Send");
    gtk_widget_add_css_class(send_button, "suggested-action");
    g_signal_connect(send_button, "clicked", G_CALLBACK(on_send_requested), ctx);
    gtk_box_append(GTK_BOX(request_bar), send_button);

    gtk_box_append(GTK_BOX(root_box), request_bar);

    /* Separator */
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_append(GTK_BOX(root_box), separator);

    /* Center Content / Workspace Area */
    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(center_box, TRUE);
    gtk_widget_set_hexpand(center_box, TRUE);

    GtkWidget *welcome_title = gtk_label_new("Reqly");
    gtk_widget_add_css_class(welcome_title, "title-1");

    GtkWidget *welcome_subtitle = gtk_label_new("Lightweight API Client");
    gtk_widget_add_css_class(welcome_subtitle, "dim-label");

    ctx->status_label = gtk_label_new("Select a method, enter a URL, and click Send.");
    gtk_widget_add_css_class(ctx->status_label, "dim-label");

    gtk_box_append(GTK_BOX(center_box), welcome_title);
    gtk_box_append(GTK_BOX(center_box), welcome_subtitle);
    gtk_box_append(GTK_BOX(center_box), ctx->status_label);

    gtk_box_append(GTK_BOX(root_box), center_box);

    gtk_window_set_child(GTK_WINDOW(window), root_box);
    gtk_window_present(GTK_WINDOW(window));

    return window;
}
