#ifndef __GUI_TEMPLATES_H_
#define __GUI_TEMPLATES_H_

#include <gtk/gtk.h>
#include <folderstate.h>
#include <stdbool.h>

GtkWidget *gui_templates_get_folder_scrollbox(GtkWidget *scrollbox, folderstate *fs, _Bool editmode, GtkWidget *);
void draw_main_window_handler(GtkWidget *window, folderstate *fs);
void gui_templates_window_set_clavis_icon(GtkWindow *window);
void gui_templates_clear_container(GtkWidget *widget);
void gui_templates_get_string(char **dest, const char *name);
void button_newfolder_handler(GtkWidget *, gpointer);
void button_goup_handler(GtkWidget *widget, gpointer data);
void gui_templates_show_about_window(GtkWidget *, gpointer);
void button_reload_handler(GtkWidget *widget, gpointer data);
void entry_filter_keyrelease_handler(GtkWidget *widget, GdkEventKey *event, gpointer data);
void entry_filter_changed_handler(GtkWidget *widget, gpointer data);
void password_decrypt_handler(GtkWidget *widget, gpointer data);
void toggle_visibility_handler(GtkWidget *widget, gpointer data);
void copy_entry_to_clipboard_handler(GtkWidget *w, gpointer);
void type_entry_with_keyboard_handler(GtkWidget *w, gpointer);
void button_newpassword_handler(GtkWidget *w, gpointer);

#endif //_GUI_TEMPLATES_H_
