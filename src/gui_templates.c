#include <gui_templates.h>
#include <stdbool.h>
#include <file_io.h>

#include <clavis_popup.h>
#include <clavis_normal.h>

#define GUI_TEMPLATES_BUTTON_WIDTH 85

#ifdef MAKE_INSTALL
  #define ABOUT_PICTURE_PNG "/usr/lib/clavis/assets/inapp_assets/about_picture.png"
#else
  #define ABOUT_PICTURE_PNG "assets/inapp_assets/about_picture.png"
#endif



//Tools
void destroy(GtkWidget *w, gpointer data){
  GtkWidget *window = (GtkWidget *) data;
  gui_templates_clear_container(window);
  gtk_widget_destroy(window);
}



//HANDLERS
void folder_button_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;
  const char *pressed = gtk_button_get_label(GTK_BUTTON(widget));
  char *pressed_fullpath = malloc(sizeof(char) * (strlen(folderstate_get_path(fs)) + strlen(pressed) + 8));
  strcpy(pressed_fullpath, folderstate_get_path(fs));
  strcat(pressed_fullpath, "/");
  strcat(pressed_fullpath, pressed);

  if (file_io_string_is_folder(pressed_fullpath)){
    folderstate_chdir(fs, pressed);
    GtkWidget *parent = gtk_widget_get_toplevel(widget);

    if (strcmp(gtk_widget_get_name(parent), "Clavis Popup") == 0){
      clavis_popup_draw_main_window(parent, (gpointer) fs);
    } else if (strcmp(gtk_widget_get_name(parent), "Clavis Normal") == 0){
      clavis_normal_draw_main_window(parent, (gpointer) fs);
    }
  } else if (file_io_string_is_file(pressed_fullpath)){
    printf("File!\n");
  } else {
    fprintf(stderr, "Error: No such file or directory: '%s'\n", pressed);
  }

  free(pressed_fullpath);
}

void button_goup_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;
  folderstate_chdir(fs, "..");
  GtkWidget *parent = gtk_widget_get_toplevel(widget);

  if (strcmp(gtk_widget_get_name(parent), "Clavis Popup") == 0){
    clavis_popup_draw_main_window(parent, (gpointer) fs);
  } else if (strcmp(gtk_widget_get_name(parent), "Clavis Normal") == 0){
    clavis_normal_draw_main_window(parent, (gpointer) fs);
  }
}

GtkWidget *gui_templates_get_folder_scrollbox(GtkWidget *scrollbox, folderstate *fs, _Bool editmode){
  int nfiles = folderstate_get_nfiles(fs);
  const char **file_list = folderstate_get_files(fs);

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  for (int i = 0; i < nfiles; i++){
    GtkWidget *folder_button = gtk_button_new_with_label(file_list[i]);
    g_signal_connect(folder_button, "pressed", G_CALLBACK(folder_button_handler), (gpointer) fs);

    const char *file_fullpath = folderstate_file_get_full_path_from_string(fs, file_list[i]);
    if (file_io_string_is_folder(file_fullpath)){
      ////Make button blue
      //GtkStyleContext *context = gtk_widget_get_style_context(folder_button);
      //gtk_style_context_add_class(context, "suggested-action");

      //Add folder icon to button
      { GtkWidget *icon = gtk_image_new_from_icon_name("folder", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(folder_button), icon); }
    }
    free((void *) file_fullpath);

    if (editmode){
      GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

      GtkWidget *button_rename = gtk_button_new_with_label("Rename");
      // { GtkWidget *icon = gtk_image_new_from_icon_name("text-x-preview", GTK_ICON_SIZE_MENU);
      // gtk_button_set_image(GTK_BUTTON(button_rename), icon); }

      GtkWidget *button_delete = gtk_button_new();
      { GtkWidget *icon = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(button_delete), icon); }

      GtkWidget *button_hbox;
      button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
      gtk_box_pack_start(GTK_BOX(button_hbox), folder_button, true, true, 0);
      gtk_box_pack_start(GTK_BOX(button_hbox), separator, false, false, 5);
      gtk_box_pack_start(GTK_BOX(button_hbox), button_rename, false, false, 0);
      gtk_box_pack_start(GTK_BOX(button_hbox), button_delete, false, false, 0);

      gtk_box_pack_start(GTK_BOX(vbox), button_hbox, false, false, 0);
    } else {
      gtk_box_pack_start(GTK_BOX(vbox), folder_button, false, false, 0);
    }
  }

  gtk_container_add(GTK_CONTAINER(scrollbox), vbox);
  gtk_widget_show_all(scrollbox);

  return scrollbox;
}

void gui_templates_clear_container(GtkWidget *window){
  GList *children, *iter;
  children = gtk_container_get_children(GTK_CONTAINER(window));
  for (iter = children; iter != NULL; iter = g_list_next(iter)){
    gtk_widget_destroy(GTK_WIDGET(iter->data));
  }
  g_list_free(children);
}

void gui_templates_show_about_window(GtkWidget *w, gpointer data){
  GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  // gui_templates_window_set_sempiternum_icon(window);

  gtk_window_set_title(window, "About Clavis");
  gtk_window_set_resizable(window, false);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_default_size(GTK_WINDOW(window), 420, 0);

  GtkAccelGroup *accel_group;

  GtkWidget *main_vbox;
  GtkWidget *final_hbox;

  GtkWidget *label_app_name;
  GtkWidget *label_version;
  GtkWidget *label_description;
  GtkWidget *label_git_repo;
  GtkWidget *label_copyright_manel;

  GtkWidget *button_credits;
  GtkWidget *button_license;
  GtkWidget *button_close;

  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ABOUT_PICTURE_PNG, NULL);
  pixbuf = gdk_pixbuf_scale_simple(pixbuf, 150, 150, GDK_INTERP_NEAREST);
  GtkWidget *image = gtk_image_new_from_pixbuf(pixbuf);

  label_app_name = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label_app_name), "<b>Clavis</b>");

  label_version = gtk_label_new(CLAVIS_VERSION);

  label_description = gtk_label_new("Clavis is an easy to use Password Manager.");
  gtk_label_set_justify(GTK_LABEL(label_description),GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap(GTK_LABEL(label_description), true);
  gtk_label_set_max_width_chars(GTK_LABEL(label_description), 50);
  gtk_label_set_width_chars(GTK_LABEL(label_description), 50);

  const char *label_git_repo_text = "<a href=\"https://github.com/ManelCG/clavis\"> Github repository</a>";
  label_git_repo = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label_git_repo), label_git_repo_text);
  gtk_widget_set_tooltip_text(label_git_repo, "Go to the Clavis Github repository");

  label_copyright_manel = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label_copyright_manel), "<small>Copyright © Manel Castillo Giménez</small>");

  button_close = gtk_button_new_with_label("Close");
  gtk_widget_set_size_request(button_close, GUI_TEMPLATES_BUTTON_WIDTH, 0);
  g_signal_connect(button_close, "clicked", G_CALLBACK(destroy), (gpointer) window);

  button_credits = gtk_button_new_with_label("Credits");
  gtk_widget_set_size_request(button_credits, GUI_TEMPLATES_BUTTON_WIDTH, 0);

  button_license = gtk_button_new_with_label("License");
  gtk_widget_set_size_request(button_license, GUI_TEMPLATES_BUTTON_WIDTH, 0);

  final_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(final_hbox), button_credits, false, false, 0);
  gtk_box_pack_start(GTK_BOX(final_hbox), button_license, false, false, 0);
  gtk_box_pack_end(GTK_BOX(final_hbox), button_close, false, false, 0);

  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), image, true, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), label_app_name, true, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), label_version, true, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), label_description, false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), label_git_repo, true, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), label_copyright_manel, true, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), final_hbox, true, false, 0);

  //accel group
  accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);
  gtk_widget_add_accelerator(button_close, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(button_close, "clicked", accel_group, GDK_KEY_q, 0, GTK_ACCEL_VISIBLE);

  gtk_container_add(GTK_CONTAINER(window), main_vbox);
  gtk_widget_show_all(GTK_WIDGET(window));

  #ifdef DEBUG_GUI_TEMPLATES
  printf("Gui templates show about window called succesfully\n");
  #endif
}
