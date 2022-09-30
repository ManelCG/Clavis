/*
 *  Clavis
 *  Copyright (C) 2022  Manel Castillo Giménez <manelcg@protonmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

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
_Bool entry_filter_keyrelease_handler(GtkWidget *widget, GdkEventKey *event, gpointer data);
void entry_filter_changed_handler(GtkWidget *widget, gpointer data);
void password_decrypt_handler(GtkWidget *widget, gpointer data);
void toggle_visibility_handler(GtkWidget *widget, gpointer data);
void folderstate_change_state_handler(GtkWidget *widget, gpointer data);

_Bool gui_templates_folder_button_from_string(GtkWidget *widget, const char *s, folderstate *fs);

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
void gui_templates_window_credits(GtkWidget *w, gpointer data);
void gui_templates_window_license(GtkWidget *w, gpointer data);

_Bool gui_templates_overwrite_confirmation(const char *path);
void gui_templates_message_dialog(const char *message);
_Bool gui_templates_yesno_dialog(const char *message, const char *cancelmessage, const char *acceptmessage);

//CLV
void gui_templates_export_clv_handler(GtkWidget *, gpointer data);
void gui_templates_import_clv_handler(GtkWidget *, gpointer data);

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
char *gui_templates_ask_for_git_credentials();

//Extra functionalities
void gui_templates_synthesize_button(GtkWidget *w, gpointer data);

#if defined (_WIN32) || defined (WIN32)
void change_theme_handler(GtkWidget *widget, gpointer data);
#endif

#endif //_GUI_TEMPLATES_H_
