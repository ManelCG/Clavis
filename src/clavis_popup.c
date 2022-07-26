#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include <stdbool.h>

#include <folderstate.h>

#include <gui_templates.h>
#include <clavis_constants.h>

void clavis_popup_draw_main_window(GtkWidget *window, gpointer data){
  gui_templates_clear_container(window);
  folderstate *fs;
  if (data == NULL){
    fs = folderstate_new(".");
  } else {
    fs = (folderstate *) data;
  }

  const char *folder_shortname = folderstate_get_path_shortname(fs);

  // --- WIDGETS ---
  //Containers
  GtkWidget *main_vbox;
  GtkWidget *bottom_hbox;
  GtkWidget *folder_scrollbox;

  //Misc
  GtkWidget *entry_filter;

  GtkWidget *button_goup;
  GtkWidget *button_close;
  GtkWidget *folder_label;


  // --- PACKING ---
  //Bottom hbox (buttons)
  folder_label = gtk_label_new(folder_shortname);

  button_goup = gtk_button_new();
  g_signal_connect(button_goup, "clicked", G_CALLBACK(button_goup_handler), (gpointer) fs);
  { GtkWidget *icon = gtk_image_new_from_icon_name("folder", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_goup), icon); }

  button_close = gtk_button_new_with_label("Close");
  g_signal_connect(button_close, "clicked", G_CALLBACK(gtk_main_quit), (gpointer) window);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_close), icon); }

  bottom_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(bottom_hbox), button_goup, false, false, 0);
  gtk_box_pack_start(GTK_BOX(bottom_hbox), folder_label, false, false, 0);
  gtk_box_pack_end(GTK_BOX(bottom_hbox), button_close, false, false, 0);


  //Main vbox
  entry_filter = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry_filter), true);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_filter), "Filter");

  folder_scrollbox = gtk_scrolled_window_new(NULL, NULL);
  gui_templates_get_folder_scrollbox(folder_scrollbox, fs, false);

  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), entry_filter, false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), folder_scrollbox, true, true, 0);
  gtk_box_pack_end(GTK_BOX(main_vbox), bottom_hbox, false, false, 0);

  free((void *) folder_shortname);

  //Pack window and draw
  gtk_container_add(GTK_CONTAINER(window), main_vbox);
  gtk_widget_show_all(window);
}

int clavis_popup_main(int argc, char *argv[]){
  gtk_init(&argc, &argv);
  GtkWidget *window_root = gtk_window_new(GTK_WINDOW_POPUP);
  gtk_window_set_title(GTK_WINDOW(window_root), "Clavis");
  gui_templates_window_set_clavis_icon(GTK_WINDOW(window_root));
  gtk_widget_set_name(window_root, CLAVIS_POPUP_MODE_NAME);
  g_signal_connect(window_root, "destroy", G_CALLBACK(gtk_main_quit), (gpointer) window_root);
  gtk_window_set_position(GTK_WINDOW(window_root), GTK_WIN_POS_CENTER);
  gtk_widget_set_can_focus(window_root, true);
  gtk_window_set_default_size(GTK_WINDOW(window_root), 480, 305);
  gtk_container_set_border_width(GTK_CONTAINER(window_root), 10);
  gtk_widget_show_all(window_root);
  clavis_popup_draw_main_window(window_root, NULL);
  gtk_main();
  return 0;
}
