#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include <stdbool.h>

#include <gui_templates.h>
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

  //Misc
  GtkWidget *entry_filter;

  GtkWidget *button_goup;
  GtkWidget *button_newfolder;
  GtkWidget *button_reload;
  GtkWidget *button_newpassword;
  GtkWidget *button_close;
  GtkWidget *folder_label;

  // --- MENU BAR ---
  GtkWidget *menu_menubar;

  //File menu
  GtkWidget *menu_filemenu;
  GtkWidget *menu_fileMi;

  //Edit menu
  GtkWidget *menu_editmenu;
  GtkWidget *menu_editMi;

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

  //File submenu
  menu_fileMi = gtk_menu_item_new_with_label("File");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_menubar), menu_fileMi);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_fileMi), menu_filemenu);

  //Edit submenu
  menu_editMi = gtk_menu_item_new_with_label("Edit");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_menubar), menu_editMi);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_editMi), menu_editmenu);

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
  folder_label = gtk_label_new(folder_label_text);

  button_newfolder = gtk_button_new();
  g_signal_connect(button_newfolder, "clicked", G_CALLBACK(button_newfolder_handler), (gpointer) fs);
  { GtkWidget *icon = gtk_image_new_from_icon_name("folder-new", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_newfolder), icon); }

  button_reload = gtk_button_new();
  g_signal_connect(button_reload, "clicked", G_CALLBACK(button_reload_handler), (gpointer) fs);
  { GtkWidget *icon = gtk_image_new_from_icon_name("view-refresh", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_reload), icon); }

  button_newpassword = gtk_button_new();
  // g_signal_connect(button_newpassword, "clicked", G_CALLBACK(button_goup_handler), (gpointer) fs);
  { GtkWidget *icon = gtk_image_new_from_icon_name("list-add", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_newpassword), icon); }

  button_goup = gtk_button_new();
  g_signal_connect(button_goup, "clicked", G_CALLBACK(button_goup_handler), (gpointer) fs);
  { GtkWidget *icon = gtk_image_new_from_icon_name("go-up", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_goup), icon); }
  if (strcmp(folderstate_get_path(fs), ".") == 0){
    gtk_widget_set_can_focus(button_goup, false);
    gtk_widget_set_sensitive(button_goup, false);
  }



  topbar_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_newfolder, false, false, 0);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_goup, false, false, 0);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_newpassword, false, false, 0);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), button_reload, false, false, 0);
  gtk_box_pack_start(GTK_BOX(topbar_hbox), folder_label, true, false, 0);


  //Bottom hbox (buttons)
  button_close = gtk_button_new_with_label("Close");
  g_signal_connect(button_close, "clicked", G_CALLBACK(gtk_main_quit), (gpointer) window);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_close), icon); }

  bottom_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_end(GTK_BOX(bottom_hbox), button_close, false, false, 10);


  //Folder vbox
  entry_filter = gtk_search_entry_new();
  gtk_widget_set_tooltip_text(entry_filter, "Filter files");
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_filter), "Filter files");
  gtk_widget_set_can_focus(entry_filter, true);
  const char *filter_text = folderstate_get_filter(fs);
  if (filter_text != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_filter), filter_text);
  }
  g_signal_connect(entry_filter, "key-release-event", G_CALLBACK(entry_filter_keyrelease_handler), (gpointer) fs);
  g_signal_connect(entry_filter, "changed", G_CALLBACK(entry_filter_changed_handler), (gpointer) fs);

  folder_scrollbox = gtk_scrolled_window_new(NULL, NULL);
  gui_templates_get_folder_scrollbox(folder_scrollbox, fs, true);

  folder_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  gtk_box_pack_start(GTK_BOX(folder_vbox), topbar_hbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
   gtk_box_pack_start(GTK_BOX(folder_vbox), separator, false, false, 0);}
  gtk_box_pack_start(GTK_BOX(folder_vbox), entry_filter, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
   gtk_box_pack_start(GTK_BOX(folder_vbox), separator, false, false, 0);}

  gtk_box_pack_start(GTK_BOX(folder_vbox), folder_scrollbox, true, true, 0);

  //Main hbox
  main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(main_hbox), folder_vbox, true, true, 10);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
   gtk_box_pack_start(GTK_BOX(main_hbox), separator, false, false, 0);}

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
  gtk_widget_show_all(window_root);
  clavis_normal_draw_main_window(window_root, NULL);
  gtk_main();
  return 0;
}
