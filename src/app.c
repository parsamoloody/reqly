#include "app.h"
#include "ui.h"
#include <gtk/gtk.h>

static void on_app_activate(GtkApplication *app, gpointer user_data)
{
    (void)user_data;
    reqly_ui_create_main_window(app);
}

int reqly_app_run(int argc, char **argv)
{
    GtkApplication *app = gtk_application_new("com.reqly.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(on_app_activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

