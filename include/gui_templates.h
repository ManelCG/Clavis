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
void entry_filter_keyrelease_handler(GtkWidget *widget, GdkEventKey *event, gpointer data);
void entry_filter_changed_handler(GtkWidget *widget, gpointer data);
void password_decrypt_handler(GtkWidget *widget, gpointer data);
void toggle_visibility_handler(GtkWidget *widget, gpointer data);

//pass init
int gui_templates_password_store_init_handler();
int gui_templates_initialize_password_store();

//Button handlers
void button_newpassword_handler(GtkWidget *w, gpointer);
void button_newfolder_handler(GtkWidget *, gpointer);
void button_goup_handler(GtkWidget *widget, gpointer data);
void button_reload_handler(GtkWidget *widget, gpointer data);

//Menu buttons
void menu_button_export_gpg_handler(GtkWidget *, gpointer);

//Special windows
void gui_templates_show_password_store_info_window(GtkWidget *, gpointer);
void gui_templates_show_about_window(GtkWidget *, gpointer);

//GPG keys
void gui_templates_export_key_handler_combobox(GtkWidget *wid, gpointer data);
void gui_templates_export_key_handler_entry(GtkWidget *wid, gpointer data);
void gui_templates_export_key_handler_label(GtkWidget *wid, gpointer data);

void gui_templates_import_key_handler();
int gui_templates_create_key_handler();

//Pâssword IO
void copy_entry_to_clipboard_handler(GtkWidget *w, gpointer);
void type_entry_with_keyboard_handler(GtkWidget *w, gpointer);

//Git
void gui_templates_pull_from_repo();
void gui_templates_push_to_repo();
void gui_templates_sync_repo();

int gui_templates_git_config_window();

//Extra functionalities
void gui_templates_synthesize_button(GtkWidget *w, gpointer data);

#endif //_GUI_TEMPLATES_H_
