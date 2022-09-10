#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include <stdbool.h>

#include <gui_templates.h>
#include <file_io.h>
#include <folderstate.h>
#include <clavis_constants.h>

void clavis_normal_draw_main_window(GtkWidget *window, gpointer data){
  gui_templates_clear_container(window);
  folderstate *fs;
  if (data == NULL){
    fs = folderstate_new(".");
  } else {
    fs = (folderstate *) data;
  }

  const char *folder_path = folderstate_get_path(fs);
  char *folder_label_text = malloc(sizeof(char) * (strlen(folder_path) + 32));
  if (strcmp(folder_path, ".") == 0){
    strcpy(folder_label_text, "");
  } else {
    strcpy(folder_label_text, folder_path+2);
  }

  // --- WIDGETS ---
  //Containers
  GtkWidget *main_vbox;
  GtkWidget *main_hbox;
  GtkWidget *topbar_hbox;
  GtkWidget *folder_vbox;
  GtkWidget *bottom_hbox;
  GtkWidget *folder_scrollbox;
  GtkWidget *folder_label_scrollbox;

  //Misc
  GtkWidget *entry_filter;

  GtkWidget *button_goup;
  GtkWidget *button_newfolder;
  GtkWidget *button_reload;
  GtkWidget *button_newpassword;
  GtkWidget *button_close;
  GtkWidget *folder_label;

  GtkWidget *password_output;

  // --- MENU BAR ---
  GtkWidget *menu_menubar;

  //File menu
  GtkWidget *menu_filemenu;
  GtkWidget *menu_fileMi;
  GtkWidget *menu_button_pass_stats;
  GtkWidget *menu_button_new_password;
  GtkWidget *menu_button_new_folder;
  GtkWidget *menu_button_export_public_gpg;
  GtkWidget *menu_button_export_private_gpg;
  GtkWidget *menu_button_quit;

  //Edit menu
  GtkWidget *menu_editmenu;
  GtkWidget *menu_editMi;
  GtkWidget *menu_config_git;
  GtkWidget *menu_button_download_git;
  GtkWidget *menu_button_upload_git;
  GtkWidget *menu_button_sync_git;
  GtkWidget *menu_button_edit_gpg;

  //Help menu
  GtkWidget *menu_helpmenu;
  GtkWidget *menu_helpMi;
  GtkWidget *menu_button_help;
  GtkWidget *menu_button_about;

  //Menu bar
  menu_menubar = gtk_menu_bar_new();
  menu_filemenu = gtk_menu_new();
  menu_editmenu = gtk_menu_new();
  menu_helpmenu = gtk_menu_new();

  //We need to initialize this button here because many other buttons emit signals on it
  #ifdef __unix__
  button_reload = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("view-refresh", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_reload), icon); }
  #elif defined(_WIN32) || defined (WIN32)
  button_reload = gtk_button_new_with_label("Refresh");
  #endif
  g_signal_connect(button_reload, "clicked", G_CALLBACK(button_reload_handler), (gpointer) fs);


  //File submenu
  menu_fileMi = gtk_menu_item_new_with_label("File");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_menubar), menu_fileMi);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_fileMi), menu_filemenu);

  {
    menu_button_pass_stats = gtk_image_menu_item_new_with_label("Password Store data");
    g_signal_connect(menu_button_pass_stats, "activate", G_CALLBACK(gui_templates_show_password_store_info_window), NULL);
    GtkWidget *icon = gtk_image_new_from_icon_name("dialog-information", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_pass_stats), icon);
  }

  {
    menu_button_export_public_gpg = gtk_image_menu_item_new_with_label("Export public GPG key");
    g_signal_connect(menu_button_export_public_gpg, "activate", G_CALLBACK(menu_button_export_gpg_handler), NULL);
    gtk_widget_set_name(menu_button_export_public_gpg, CLAVIS_BUTTON_EXPORT_PUBLIC_KEY_NAME);
    GtkWidget *icon = gtk_image_new_from_icon_name("document-save-as", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_export_public_gpg), icon);
  }

  {
    menu_button_export_private_gpg = gtk_image_menu_item_new_with_label("Export private GPG key");
    g_signal_connect(menu_button_export_private_gpg, "activate", G_CALLBACK(menu_button_export_gpg_handler), NULL);
    gtk_widget_set_name(menu_button_export_private_gpg, CLAVIS_BUTTON_EXPORT_PRIVATE_KEY_NAME);
    GtkWidget *icon = gtk_image_new_from_icon_name("document-save-as", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_export_private_gpg), icon);
  }


  {
    menu_button_new_folder = gtk_image_menu_item_new_with_label("New folder");
    g_signal_connect(menu_button_new_folder, "activate", G_CALLBACK(button_newfolder_handler), (gpointer) fs);
    g_signal_connect(menu_button_new_folder, "activate", G_CALLBACK(gui_templates_synthesize_button), (gpointer) button_reload);
    GtkWidget *icon = gtk_image_new_from_icon_name("folder-new", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_new_folder), icon);
  }

  {
    menu_button_new_password = gtk_image_menu_item_new_with_label("New password");
    g_signal_connect(menu_button_new_password, "activate", G_CALLBACK(button_newpassword_handler), (gpointer) fs);
    g_signal_connect(menu_button_new_password, "activate", G_CALLBACK(gui_templates_synthesize_button), (gpointer) button_reload);
    gtk_widget_set_name(menu_button_new_password, CLAVIS_BUTTON_NEWPASSWORD_NAME);
    GtkWidget *icon = gtk_image_new_from_icon_name("list-add", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_new_password), icon);
  }

  {
    menu_button_quit = gtk_image_menu_item_new_with_label("Quit");
    g_signal_connect(menu_button_quit, "activate", G_CALLBACK(gtk_main_quit), NULL);
    GtkWidget *icon = gtk_image_new_from_icon_name("window-close", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_quit), icon);
  }


  gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), menu_button_pass_stats);
  {GtkWidget *separator = gtk_separator_menu_item_new();
   gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), separator);}

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), menu_button_new_folder);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), menu_button_new_password);
  {GtkWidget *separator = gtk_separator_menu_item_new();
   gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), separator);}

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), menu_button_export_public_gpg);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), menu_button_export_private_gpg);
  {GtkWidget *separator = gtk_separator_menu_item_new();
   gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), separator);}

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_filemenu), menu_button_quit);

  //Edit submenu
  menu_editMi = gtk_menu_item_new_with_label("Edit");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_menubar), menu_editMi);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_editMi), menu_editmenu);

  {
    menu_button_edit_gpg = gtk_image_menu_item_new_with_label("GPG key settings");
    g_signal_connect(menu_button_edit_gpg, "activate", G_CALLBACK(gui_templates_initialize_password_store), NULL);
    GtkWidget *icon = gtk_image_new_from_icon_name("channel-secure", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_edit_gpg), icon);
  }

  {
    menu_button_download_git = gtk_image_menu_item_new_with_label("Download passwords from Git");
    g_signal_connect(menu_button_download_git, "activate", G_CALLBACK(gui_templates_pull_from_repo), NULL);
    g_signal_connect(menu_button_download_git, "activate", G_CALLBACK(gui_templates_synthesize_button), (gpointer) button_reload);
    GtkWidget *icon = gtk_image_new_from_icon_name("go-down", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_download_git), icon);
  }

  {
    menu_button_upload_git = gtk_image_menu_item_new_with_label("Upload passwords to Git");
    g_signal_connect(menu_button_upload_git, "activate", G_CALLBACK(gui_templates_push_to_repo), NULL);
    g_signal_connect(menu_button_upload_git, "activate", G_CALLBACK(gui_templates_synthesize_button), (gpointer) button_reload);
    GtkWidget *icon = gtk_image_new_from_icon_name("go-up", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_upload_git), icon);
  }

  {
    menu_button_sync_git = gtk_image_menu_item_new_with_label("Sync all passwords");
    g_signal_connect(menu_button_sync_git, "activate", G_CALLBACK(gui_templates_sync_repo), NULL);
    g_signal_connect(menu_button_sync_git, "activate", G_CALLBACK(gui_templates_synthesize_button), (gpointer) button_reload);
    GtkWidget *icon = gtk_image_new_from_icon_name("view-refresh", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_sync_git), icon);
  }

  {
    menu_config_git = gtk_image_menu_item_new_with_label("Git server settings");
    g_signal_connect(menu_config_git, "activate", G_CALLBACK(gui_templates_git_config_window), NULL);
    GtkWidget *icon = gtk_image_new_from_icon_name("emblem-system", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_config_git), icon);
  }

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_editmenu), menu_button_upload_git);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_editmenu), menu_button_download_git);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_editmenu), menu_button_sync_git);
  {GtkWidget *separator = gtk_separator_menu_item_new();
   gtk_menu_shell_append(GTK_MENU_SHELL(menu_editmenu), separator);}
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_editmenu), menu_config_git);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_editmenu), menu_button_edit_gpg);

  //Help submenu
  menu_helpMi = gtk_menu_item_new_with_label("Help");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_menubar), menu_helpMi);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_helpMi), menu_helpmenu);

  {
    menu_button_help = gtk_image_menu_item_new_with_label("Help");
    // g_signal_connect(menu_button_help, "activate", G_CALLBACK(gui_templates_show_help_window), NULL);
    GtkWidget *icon = gtk_image_new_from_icon_name("help-contents", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_help), icon);
  }

  {
    menu_button_about = gtk_image_menu_item_new_with_label("About");
    GtkWidget *icon = gtk_image_new_from_icon_name("help-about", 16);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_button_about), icon);
    g_signal_connect(menu_button_about, "activate", G_CALLBACK(gui_templates_show_about_window), NULL);
  }

  gtk_menu_shell_append(GTK_MENU_SHELL(menu_helpmenu), menu_button_help);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_helpmenu), menu_button_about);



  // --- PACKING ---
  //topbar hbox (info, buttons)
  folder_label_scrollbox = gtk_scrolled_window_new(NULL, NULL);
  folder_label = gtk_label_new(folder_label_text);
  gtk_container_add(GTK_CONTAINER(folder_label_scrollbox), folder_label);
  gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(folder_label_scrollbox), 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(folder_label_scrollbox), GTK_POLICY_EXTERNAL, GTK_POLICY_NEVER);

  #ifdef __unix__
  button_newfolder = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("folder-new", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_newfolder), icon); }
  #elif defined(_WIN32) || defined (WIN32)
  button_newfolder = gtk_button_new_with_label("New folder");
  #endif
  g_signal_connect(button_newfolder, "clicked", G_CALLBACK(button_newfolder_handler), (gpointer) fs);

  #ifdef __unix__
  button_newpassword = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("list-add", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_newpassword), icon); }
  #elif defined(_WIN32) || defined (WIN32)
  button_newpassword = gtk_button_new_with_label("Add new password");
  #endif
  gtk_widget_set_name(button_newpassword, CLAVIS_BUTTON_NEWPASSWORD_NAME);
  g_signal_connect(button_newpassword, "clicked", G_CALLBACK(button_newpassword_handler), (gpointer) fs);

  #ifdef __unix__
  button_goup = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("go-up", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_goup), icon); }
  #elif defined(_WIN32) || defined (WIN32)
  button_goup = gtk_button_new_with_label("Go up");
  #endif
  g_signal_connect(button_goup, "clicked", G_CALLBACK(button_goup_handler), (gpointer) fs);
  if (strcmp(folderstate_get_path(fs), ".") == 0){
    gtk_widget_set_can_focus(button_goup, false);
    gtk_widget_set_sensitive(button_goup, false);
  }



  topbar_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_newfolder, false, false, 0);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_goup, false, false, 0);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_newpassword, false, false, 0);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_reload, false, false, 0);
  #ifdef __unix__
  gtk_box_pack_start(GTK_BOX(topbar_hbox), folder_label_scrollbox, true, true, 0);
  #endif


  //Bottom hbox (buttons)
  button_close = gtk_button_new_with_label("Close");
  g_signal_connect(button_close, "clicked", G_CALLBACK(gtk_main_quit), (gpointer) window);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_close), icon); }

  bottom_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_end(GTK_BOX(bottom_hbox), button_close, false, false, 10);


  //Folder vbox
  #ifdef __unix__
  entry_filter = gtk_search_entry_new();
  #elif defined(_WIN32) || defined (WIN32)
  entry_filter = gtk_entry_new();
  #endif
  gtk_widget_set_tooltip_text(entry_filter, "Filter files");
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_filter), "Filter files");
  gtk_widget_set_can_focus(entry_filter, true);
  const char *filter_text = folderstate_get_filter(fs);
  if (filter_text != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_filter), filter_text);
  }
  g_signal_connect(entry_filter, "key-release-event", G_CALLBACK(entry_filter_keyrelease_handler), (gpointer) fs);
  g_signal_connect(entry_filter, "changed", G_CALLBACK(entry_filter_changed_handler), (gpointer) fs);

  //Password output entry
  GtkWidget *password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  password_output = gtk_entry_new();
  gtk_entry_set_visibility(GTK_ENTRY(password_output), false);
  gtk_editable_set_editable(GTK_EDITABLE(password_output), false);
  gtk_entry_set_placeholder_text(GTK_ENTRY(password_output), "Password output");
  #ifdef __unix__
  GtkWidget *button_copy = gtk_button_new();
  #elif defined(_WIN32) || defined (WIN32)
  GtkWidget *button_copy = gtk_button_new_with_label("Copy");
  #endif
  { GtkWidget *icon = gtk_image_new_from_icon_name("edit-copy", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_copy), icon); }
  g_signal_connect(button_copy, "pressed", G_CALLBACK(copy_entry_to_clipboard_handler), (gpointer) password_output);

  #ifdef __unix__
  GtkWidget *button_xdotool = gtk_button_new();
  #elif defined(_WIN32) || defined (WIN32)
  GtkWidget *button_xdotool = gtk_button_new_with_label("Type");
  #endif
  { GtkWidget *icon = gtk_image_new_from_icon_name("document-edit", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_xdotool), icon); }
  g_signal_connect(button_xdotool, "pressed", G_CALLBACK(type_entry_with_keyboard_handler), (gpointer) password_output);

  GtkWidget *toggle_visibility = gtk_check_button_new_with_label("Show pass");
  g_signal_connect(toggle_visibility, "toggled", G_CALLBACK(toggle_visibility_handler), (gpointer) password_output);
  gtk_box_pack_start(GTK_BOX(password_hbox), password_output, true, true, 0);
  gtk_box_pack_start(GTK_BOX(password_hbox), button_copy, false, false, 0);
  gtk_box_pack_start(GTK_BOX(password_hbox), button_xdotool, false, false, 0);
  gtk_box_pack_start(GTK_BOX(password_hbox), toggle_visibility, false, false, 0);

  folder_scrollbox = gtk_scrolled_window_new(NULL, NULL);
  gui_templates_get_folder_scrollbox(folder_scrollbox, fs, true, password_output);

  folder_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  gtk_box_pack_start(GTK_BOX(folder_vbox), topbar_hbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
   gtk_box_pack_start(GTK_BOX(folder_vbox), separator, false, false, 0);}
  #if defined(_WIN32) || defined (WIN32)
  gtk_box_pack_start(GTK_BOX(folder_vbox), folder_label_scrollbox, false, false, 0);
  #endif
  gtk_box_pack_start(GTK_BOX(folder_vbox), entry_filter, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
   gtk_box_pack_start(GTK_BOX(folder_vbox), separator, false, false, 0);}

  gtk_box_pack_start(GTK_BOX(folder_vbox), folder_scrollbox, true, true, 0);


  //Password label and output + separator
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
   gtk_box_pack_start(GTK_BOX(folder_vbox), separator, false, false, 0);}
  GtkWidget *password_label = gtk_label_new("Decrypted password:");
  gtk_box_pack_start(GTK_BOX(folder_vbox), password_label, false, false, 0);
  gtk_box_pack_start(GTK_BOX(folder_vbox), password_hbox, false, false, 0);


  //Main hbox
  main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(main_hbox), folder_vbox, true, true, 10);
  // {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
  //  gtk_box_pack_start(GTK_BOX(main_hbox), separator, false, false, 0);}

  //Main vbox
  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), menu_menubar, false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), main_hbox, true, true, 0);
  gtk_box_pack_end(GTK_BOX(main_vbox), bottom_hbox, false, false, 10);

  gtk_container_add(GTK_CONTAINER(window), main_vbox);
  gtk_widget_show_all(window);
}

int clavis_normal_main(int argc, char *argv[]){
  gtk_init(&argc, &argv);
  GtkWidget *window_root = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window_root), "Clavis");
  gui_templates_window_set_clavis_icon(GTK_WINDOW(window_root));
  gtk_widget_set_name(window_root, CLAVIS_NORMAL_MODE_NAME);
  g_signal_connect(window_root, "destroy", G_CALLBACK(gtk_main_quit), (gpointer) window_root);
  gtk_window_set_position(GTK_WINDOW(window_root), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(window_root), 0);
  gtk_window_set_default_size(GTK_WINDOW(window_root), 350, 650);

  if (gui_templates_password_store_init_handler() != 0){
    exit(-1);
  }

  gtk_widget_show_all(window_root);
  clavis_normal_draw_main_window(window_root, NULL);


  gtk_main();
  return 0;
}
