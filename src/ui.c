#include "ui.h"

GtkWidget *reqly_ui_create_main_window(GtkApplication *app)
{
    GtkWidget *window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Reqly");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);

    GtkWidget *header_bar = gtk_header_bar_new();
    GtkWidget *title_widget = gtk_window_title_new("Reqly", NULL);
    gtk_header_bar_set_title_widget(GTK_HEADER_BAR(header_bar), title_widget);
    gtk_window_set_titlebar(GTK_WINDOW(window), header_bar);

    GtkWidget *root_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    GtkWidget *center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_vexpand(center_box, TRUE);
    gtk_widget_set_hexpand(center_box, TRUE);

    GtkWidget *title_label = gtk_label_new("Reqly");
    gtk_widget_add_css_class(title_label, "title-1");

    GtkWidget *subtitle_label = gtk_label_new("Lightweight API Client");
    gtk_widget_add_css_class(subtitle_label, "dim-label");

    gtk_box_append(GTK_BOX(center_box), title_label);
    gtk_box_append(GTK_BOX(center_box), subtitle_label);

    gtk_box_append(GTK_BOX(root_box), center_box);

    gtk_window_set_child(GTK_WINDOW(window), root_box);
    gtk_window_present(GTK_WINDOW(window));

    return window;
}
