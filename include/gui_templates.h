#ifndef __GUI_TEMPLATES_H_
#define __GUI_TEMPLATES_H_

#include <gtk/gtk.h>
#include <folderstate.h>
#include <stdbool.h>

GtkWidget *gui_templates_get_folder_scrollbox(GtkWidget *scrollbox, folderstate *fs, _Bool editmode);
void draw_main_window_handler(GtkWidget *window, folderstate *fs);
void gui_templates_clear_container(GtkWidget *widget);
void gui_templates_get_string(char **dest, const char *name);
void button_newfolder_handler(GtkWidget *, gpointer);
void button_goup_handler(GtkWidget *widget, gpointer data);
void gui_templates_show_about_window(GtkWidget *, gpointer);

#endif //_GUI_TEMPLATES_H_
