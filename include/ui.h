#ifndef REQLY_UI_H
#define REQLY_UI_H

#include <gtk/gtk.h>

/**
 * Creates, configures, and presents the main application window.
 *
 * @param app The active GtkApplication instance.
 * @return Pointer to the newly created main window widget.
 */
GtkWidget *reqly_ui_create_main_window(GtkApplication *app);

#endif /* REQLY_UI_H */

