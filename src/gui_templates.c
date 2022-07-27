#include <gui_templates.h>
#include <stdbool.h>
#include <file_io.h>
#include <unistd.h>
#include <stdio.h>

#include <clavis_constants.h>
#include <clavis_passgen.h>

#include <clavis_popup.h>
#include <clavis_normal.h>

#include <sys/wait.h>

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

void gui_templates_window_set_clavis_icon(GtkWindow *window){
  GdkPixbuf *window_icon = gdk_pixbuf_new_from_file(ABOUT_PICTURE_PNG, NULL);
  window_icon = gdk_pixbuf_scale_simple(window_icon, 16, 16, GDK_INTERP_NEAREST);
  gtk_window_set_icon(window, window_icon);
}


//HANDLERS
void copy_entry_to_clipboard_handler(GtkWidget *widget, gpointer data){
  GtkWidget *entry = (GtkWidget *) data;
  const char *pw = gtk_entry_get_text(GTK_ENTRY(entry));
  if (strlen(pw) != 0){
    #ifdef __unix__
      int p[2];
      if (pipe(p) < 0){
        perror("Could not pipe");
        exit(-1);
      }
      int pid = fork();
      if (pid < 0){
        perror("Could not fork");
        exit(pid);
      }
      if (pid == 0){  //Child
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);

        execlp("xclip", "xclip", "-sel", "clip", NULL);
        printf("Post execlp ERROR\n");
        exit(-1);
      }
      //Parent
      close(p[0]);
      write(p[1], pw, strlen(pw));
      close(p[1]);

    #elif defined(_WIN32) || defined (WIN32)
      printf("Clipboard in Windows is still WIP!\n");
    #endif
  }
}
void type_entry_with_keyboard_handler(GtkWidget *widget, gpointer data){
  GtkWidget *entry = (GtkWidget *) data;
  const char *pw = gtk_entry_get_text(GTK_ENTRY(entry));
  if (strlen(pw) != 0){
    #ifdef __unix__
      int p[2];
      if (pipe(p) < 0){
        perror("Could not pipe");
        exit(-1);
      }
      int pid = fork();
      if (pid < 0){
        perror("Could not fork");
        exit(pid);
      }
      if (pid == 0){  //Child
        close(0);
        dup(p[0]);
        close(p[0]);
        close(p[1]);

        execlp("xdotool", "xdotool", "type", "--clearmodifiers", "--file", "-", NULL);
        printf("Post execlp ERROR\n");
        exit(-1);
      }
      //Parent
      gtk_main_quit();
      close(p[0]);
      write(p[1], pw, strlen(pw));
      close(p[1]);

    #elif defined(_WIN32) || defined (WIN32)
      printf("Autotyping in Windows is still WIP!\n");
    #endif
  }
}
void toggle_visibility_handler(GtkWidget *widget, gpointer data){
  GtkWidget *entry = (GtkWidget *) data;
  gtk_entry_set_visibility(GTK_ENTRY(entry), gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}
void password_decrypt_handler(GtkWidget *widget, gpointer data){
  printf("Decrypting\n");
}
void entry_filter_keyrelease_handler(GtkWidget *widget, GdkEventKey *event, gpointer data){
  if (strcmp(gdk_keyval_name (event->keyval), "Return") == 0){
    folderstate *fs = (folderstate *) data;
    folderstate_set_filter(fs, gtk_entry_get_text(GTK_ENTRY(widget)));
    folderstate_reload(fs);


    GtkWidget *parent = gtk_widget_get_toplevel(widget);
    draw_main_window_handler(parent, fs);
  }
}
void entry_filter_changed_handler(GtkWidget *widget, gpointer data){
  if (strcmp(gtk_entry_get_text(GTK_ENTRY(widget)), "") == 0){
    folderstate *fs = (folderstate *) data;
    folderstate_set_filter(fs, "");
    folderstate_reload(fs);


    GtkWidget *parent = gtk_widget_get_toplevel(widget);
    draw_main_window_handler(parent, fs);
  }
}
void passgen_button_handler(GtkWidget *widget, gpointer data){
  passgen_generate_new_password((passgen *) data);
}
void toggle_numerals_handler(GtkWidget *widget, gpointer data){
  passgen *pg = (passgen *) data;
  passgen_set_numerals(pg, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}
void toggle_symbols_handler(GtkWidget *widget, gpointer data){
  passgen *pg = (passgen *) data;
  passgen_set_symbols(pg, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}
void toggle_pronunceable_handler(GtkWidget *widget, gpointer data){
  passgen *pg = (passgen *) data;
  passgen_set_pronunceable(pg, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}
void toggle_uppercase_handler(GtkWidget *widget, gpointer data){
  passgen *pg = (passgen *) data;
  passgen_set_uppercase(pg, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}
void toggle_lowercase_handler(GtkWidget *widget, gpointer data){
  passgen *pg = (passgen *) data;
  passgen_set_lowercase(pg, gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget)));
}
void set_passlen_handler(GtkWidget *widget, gpointer data){
  passgen *pg = (passgen *) data;
  passgen_set_len(pg, atoi(gtk_entry_get_text(GTK_ENTRY(widget))));
}
void button_newpassword_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;
  passgen *pg = passgen_new();

  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Add new password:");
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Add password");
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }

  //Boxes
  GtkWidget *main_hbox;

  GtkWidget *left_vbox;
  GtkWidget *right_vbox;

  GtkWidget *passlen_hbox;
  GtkWidget *hbox_of_vbox;
  GtkWidget *gen_left_vbox;
  GtkWidget *gen_right_vbox;

  GtkWidget *password_hbox;
  GtkWidget *saveto_vbox;

  //Widgets
  GtkWidget *entry_password;

  GtkWidget *toggle_visibility;
  GtkWidget *button_generate;

  GtkWidget *entry_passlen;
  GtkWidget *check_numerals;
  GtkWidget *check_symbols;
  GtkWidget *check_pron;
  GtkWidget *check_upper;
  GtkWidget *check_lower;

  GtkWidget *label_saveto;
  GtkWidget *entry_passname;

  //Password hbox
  entry_password = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), "Password");
  passgen_set_output_entry(pg, entry_password);

  button_generate = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("view-refresh", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_generate), icon); }
  gtk_widget_set_tooltip_text(button_generate, "Generate password with options");
  g_signal_connect(button_generate, "clicked", G_CALLBACK(passgen_button_handler), (gpointer) pg);

  password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(password_hbox), entry_password, true, true, 0);
  gtk_box_pack_start(GTK_BOX(password_hbox), button_generate, false, false, 0);

  //Saveto hbox
  label_saveto = gtk_label_new("Set password name:");
  entry_passname = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_passname), "New password name");
  saveto_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(saveto_vbox), label_saveto, false, false, 0);
  gtk_box_pack_start(GTK_BOX(saveto_vbox), entry_passname, false, false, 0);

  gtk_entry_set_text(GTK_ENTRY(entry_passname), "new_password");


  //Main left vbox
  GtkWidget *new_pass_label = gtk_label_new("New password:");
  left_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(left_vbox), new_pass_label, false, false, 5);
  gtk_box_pack_start(GTK_BOX(left_vbox), password_hbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(left_vbox), separator, false, false, 5);}
  gtk_box_pack_start(GTK_BOX(left_vbox), saveto_vbox, false, false, 0);

  //Main right vbox
  GtkWidget *label_passlen = gtk_label_new("Password length:");
  entry_passlen = gtk_entry_new();
  check_numerals = gtk_check_button_new_with_label("Numerals");
  check_symbols = gtk_check_button_new_with_label("Symbols");
  check_pron = gtk_check_button_new_with_label("Pronounceable");
  check_upper = gtk_check_button_new_with_label("Uppercase");
  check_lower = gtk_check_button_new_with_label("Lowercase");

  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_passlen), "Password length");
  char buffer[128];
  sprintf(buffer, "%d", passgen_get_len(pg));
  gtk_entry_set_text(GTK_ENTRY(entry_passlen), buffer);

  g_signal_connect(entry_passlen, "changed", G_CALLBACK(set_passlen_handler), (gpointer) pg);
  g_signal_connect(check_numerals, "toggled", G_CALLBACK(toggle_numerals_handler), (gpointer) pg);
  g_signal_connect(check_symbols, "toggled", G_CALLBACK(toggle_symbols_handler), (gpointer) pg);
  g_signal_connect(check_pron, "toggled", G_CALLBACK(toggle_pronunceable_handler), (gpointer) pg);
  g_signal_connect(check_upper, "toggled", G_CALLBACK(toggle_uppercase_handler), (gpointer) pg);
  g_signal_connect(check_lower, "toggled", G_CALLBACK(toggle_lowercase_handler), (gpointer) pg);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_numerals), passgen_get_numerals(pg));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_symbols), passgen_get_symbols(pg));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_pron), passgen_get_pronunceable(pg));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_upper), passgen_get_uppercase(pg));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_lower), passgen_get_lowercase(pg));

  passlen_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(passlen_hbox), label_passlen, false, false, 0);
  gtk_box_pack_start(GTK_BOX(passlen_hbox), entry_passlen, true, true, 0);

  gen_left_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(gen_left_vbox), check_lower, false, false, 0);
  gtk_box_pack_start(GTK_BOX(gen_left_vbox), check_numerals, false, false, 0);
  gtk_box_pack_start(GTK_BOX(gen_left_vbox), check_pron, false, false, 0);

  gen_right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(gen_right_vbox), check_upper, false, false, 0);
  gtk_box_pack_start(GTK_BOX(gen_right_vbox), check_symbols, false, false, 0);

  hbox_of_vbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(hbox_of_vbox), gen_left_vbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
  gtk_box_pack_start(GTK_BOX(hbox_of_vbox), separator, false, false, 15);}
  gtk_box_pack_start(GTK_BOX(hbox_of_vbox), gen_right_vbox, false, false, 0);

  GtkWidget *label_autogen = gtk_label_new("Password generator:");
  right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_pack_start(GTK_BOX(right_vbox), label_autogen, false, false, 0);
  gtk_box_pack_start(GTK_BOX(right_vbox), passlen_hbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(right_vbox), separator, false, false, 15);}
  gtk_box_pack_start(GTK_BOX(right_vbox), hbox_of_vbox, false, false, 0);

  //Main hbox
  main_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_pack_start(GTK_BOX(main_hbox), left_vbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
  gtk_box_pack_start(GTK_BOX(main_hbox), separator, false, false, 15);}
  gtk_box_pack_start(GTK_BOX(main_hbox), right_vbox, false, false, 0);


  gtk_box_pack_start(GTK_BOX(dialog_box), main_hbox, false, false, 0);
  gtk_widget_show_all(dialog);
  int response = gtk_dialog_run(GTK_DIALOG(dialog));

  if (response == GTK_RESPONSE_OK){
    const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
    const char *name = folderstate_file_get_full_path_from_string(fs, gtk_entry_get_text(GTK_ENTRY(entry_passname)));
    file_io_encrypt_password(password, name);
  }

  destroy(dialog, dialog);

  folderstate_reload(fs);
  GtkWidget *parent = gtk_widget_get_toplevel(widget);
  draw_main_window_handler(parent, fs);

}
void button_newfolder_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;

  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "New folder name:");
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  GtkWidget *entry = gtk_entry_new();
  gtk_box_pack_end(GTK_BOX(dialog_box), entry, false, false, 0);

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }

  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Create");
  { GtkWidget *icon = gtk_image_new_from_icon_name("folder-new", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }

  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
  gtk_widget_add_accelerator(dialog_button_ok, "clicked", accel_group, GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);

  gtk_widget_show_all(dialog);

  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  const char *text = gtk_entry_get_text(GTK_ENTRY(entry));

  if (response == GTK_RESPONSE_OK && strcmp(text, "") != 0){
    const char *path = folderstate_file_get_full_path_from_string(fs, text);
    char dialog_prompt[strlen(path) + 128];
    _Bool should_create = true;

    if (file_io_string_is_file(path) || file_io_string_is_folder(path)){
      should_create = false;

      sprintf(dialog_prompt, "%s already exists. Replace?", path);
      GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
      gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

      GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
      { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Replace");
      { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }

      GtkAccelGroup *accel_group = gtk_accel_group_new();
      gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
      gtk_widget_add_accelerator(dialog_button_ok, "clicked", accel_group, GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);
      gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);

      int response = gtk_dialog_run(GTK_DIALOG(dialog));

      if (response == GTK_RESPONSE_OK){
        if (file_io_rm_rf(path)){
          should_create = true;
        }
      }

      destroy(dialog, dialog);
    }

    if (should_create){
      if (mkdir_handler(path) != 0){
        sprintf(dialog_prompt, "Could not overwrite %s.", path);
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, dialog_prompt);
        gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
        gtk_dialog_run(GTK_DIALOG(dialog));

        destroy(dialog, dialog);
      }
    }

    folderstate_reload(fs);

    GtkWidget *parent = gtk_widget_get_toplevel(widget);
    draw_main_window_handler(parent, fs);
  }

  destroy(dialog, dialog);
}

void button_rename_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;
  const char *name = gtk_widget_get_name(widget);
  const char *oldpath = folderstate_file_get_full_path_from_string(fs, name);

  char dialog_prompt[strlen(oldpath) + 128];
  sprintf(dialog_prompt, "Rename %s:\n", oldpath);
  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  GtkWidget *entry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry), name);
  gtk_box_pack_end(GTK_BOX(dialog_box), entry, false, false, 0);

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }

  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Rename");
  { GtkWidget *icon = gtk_image_new_from_icon_name("document-edit", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }

  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
  gtk_widget_add_accelerator(dialog_button_ok, "clicked", accel_group, GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);

  gtk_widget_show_all(dialog);

  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  const char *text = gtk_entry_get_text(GTK_ENTRY(entry));

  if (response == GTK_RESPONSE_OK && strcmp(text, "") != 0 && strcmp(text, name) != 0){
    const char *path = folderstate_file_get_full_path_from_string(fs, text);
    _Bool should_rename = true;

    if (file_io_string_is_file(path) || file_io_string_is_folder(path)){
      should_rename = false;

      sprintf(dialog_prompt, "%s already exists. Replace?", path);
      GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
      gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

      GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
      { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Replace");
      { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }

      GtkAccelGroup *accel_group = gtk_accel_group_new();
      gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
      gtk_widget_add_accelerator(dialog_button_ok, "clicked", accel_group, GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);
      gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);

      int response = gtk_dialog_run(GTK_DIALOG(dialog));

      if (response == GTK_RESPONSE_OK){
        if (file_io_rm_rf(path)){
          should_rename = true;
        }
      }

      destroy(dialog, dialog);
    }

    if (should_rename){
      if (rename(oldpath, path) != 0){
        sprintf(dialog_prompt, "Could not overwrite %s.", path);
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, dialog_prompt);
        gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
        gtk_dialog_run(GTK_DIALOG(dialog));

        destroy(dialog, dialog);
      }
    }

    folderstate_reload(fs);

    GtkWidget *parent = gtk_widget_get_toplevel(widget);
    draw_main_window_handler(parent, fs);
  }

  destroy(dialog, dialog);
}
void button_delete_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;
  const char *name = gtk_widget_get_name(widget);
  const char *path = folderstate_file_get_full_path_from_string(fs, name);

  _Bool deleted = false;

  if (file_io_string_is_folder(path)){
    char dialog_prompt[strlen(path) + 128];
    sprintf(dialog_prompt, "Folder %s contains %d files. Remove anyways?", path, file_io_folder_get_file_n(path, ""));
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

    GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }

    GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Delete");
    // { GtkWidget *icon = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_MENU);
    // gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
    GtkStyleContext *context = gtk_widget_get_style_context(dialog_button_ok);
    gtk_style_context_add_class(context, "destructive-action");

    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
    gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);

    int response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK){
      if (!file_io_rm_rf(path)){
        char dialog_prompt[strlen(path) + 128];
        sprintf(dialog_prompt, "Error. Could not remove %s", path);
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, dialog_prompt);
        gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
        gtk_dialog_run(GTK_DIALOG(dialog));
      } else {
        deleted = true;
      }
    }
    destroy(dialog, dialog);
  } else if (file_io_string_is_file(path)){
    char dialog_prompt[strlen(path) + 128];
    sprintf(dialog_prompt, "Remove file '%s'? This cannot be undone!", path);
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

    GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }

    GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Delete");
    // { GtkWidget *icon = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_MENU);
    // gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
    GtkStyleContext *context = gtk_widget_get_style_context(dialog_button_ok);
    gtk_style_context_add_class(context, "destructive-action");

    int response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK){
      remove(path);
      deleted = true;
    }
    destroy(dialog, dialog);
  }

  if (deleted){
    folderstate_reload(fs);

    GtkWidget *parent = gtk_widget_get_toplevel(widget);
    draw_main_window_handler(parent, fs);
  }
}

void button_reload_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;

  folderstate_reload(fs);
  GtkWidget *parent = gtk_widget_get_toplevel(widget);
  draw_main_window_handler(parent, fs);
}

void draw_main_window_handler(GtkWidget *window, folderstate *fs){
  const char *widgetname = gtk_widget_get_name(window);
  if (strcmp(widgetname, CLAVIS_NORMAL_MODE_NAME) == 0){
    clavis_normal_draw_main_window(window, (gpointer) fs);
  } else if (strcmp(widgetname, CLAVIS_POPUP_MODE_NAME) == 0){
    clavis_popup_draw_main_window(window, (gpointer) fs);
  }
}

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
    draw_main_window_handler(parent, fs);
  }

  free(pressed_fullpath);
}
void file_button_handler(GtkWidget *widget, gpointer data){
  GtkWidget *label = (GtkWidget *) data;
  const char *name = gtk_widget_get_name(widget);
  if (file_io_string_is_file(name)){
    const char *pass = file_io_decrypt_password(name);
    gtk_entry_set_text(GTK_ENTRY(label), pass);
    free((char *) pass);
  }
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

GtkWidget *gui_templates_get_folder_scrollbox(GtkWidget *scrollbox, folderstate *fs, _Bool editmode, GtkWidget *output_widget){
  int nfiles = folderstate_get_nfiles(fs);
  const char **file_list = folderstate_get_files(fs);
  _Bool files_section = false;
  _Bool first_file = true;
  _Bool has_folders = false;

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  for (int i = 0; i < nfiles; i++){
    GtkWidget *folder_button = gtk_button_new_with_label(file_list[i]);
    gtk_button_set_alignment(GTK_BUTTON(folder_button), 0, 0.5);
    gtk_widget_set_name(folder_button, folderstate_file_get_full_path_from_string(fs, file_list[i]));

    const char *file_fullpath = folderstate_file_get_full_path_from_string(fs, file_list[i]);
    if (file_io_string_is_folder(file_fullpath)){
      g_signal_connect(folder_button, "activate", G_CALLBACK(folder_button_handler), (gpointer) fs);
      g_signal_connect(folder_button, "pressed", G_CALLBACK(folder_button_handler), (gpointer) fs);
      has_folders = true;
      ////Make button blue
      //GtkStyleContext *context = gtk_widget_get_style_context(folder_button);
      //gtk_style_context_add_class(context, "suggested-action");

      //Add folder icon to button
      { GtkWidget *icon = gtk_image_new_from_icon_name("folder", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(folder_button), icon); }
    } else if (file_io_string_is_file(file_fullpath)){
      g_signal_connect(folder_button, "activate", G_CALLBACK(file_button_handler), (gpointer) output_widget);
      g_signal_connect(folder_button, "pressed", G_CALLBACK(file_button_handler), (gpointer) output_widget);
      files_section = true;
      //Add folder icon to button
      if (strlen(file_fullpath) > 4 && strcmp(&file_fullpath[strlen(file_fullpath)-4], ".gpg") == 0){
        { GtkWidget *icon = gtk_image_new_from_icon_name("channel-secure", GTK_ICON_SIZE_MENU);
        gtk_button_set_image(GTK_BUTTON(folder_button), icon); }
      } else {

        { GtkWidget *icon = gtk_image_new_from_icon_name("text-x-generic", GTK_ICON_SIZE_MENU);
        gtk_button_set_image(GTK_BUTTON(folder_button), icon); }
      }
    }

    if (files_section && first_file && has_folders){
      first_file = false;
      GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
      gtk_box_pack_start(GTK_BOX(vbox), separator, false, false, 5);
    }

    if (editmode){
      GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

      GtkWidget *button_rename = gtk_button_new();
      gtk_widget_set_tooltip_text(button_rename, "Rename file");
      { GtkWidget *icon = gtk_image_new_from_icon_name("document-edit", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(button_rename), icon); }
      gtk_widget_set_name(button_rename, file_list[i]);
      g_signal_connect(button_rename, "pressed", G_CALLBACK(button_rename_handler), (gpointer) fs);

      GtkWidget *button_delete = gtk_button_new();
      gtk_widget_set_tooltip_text(button_delete, "Delete");
      { GtkWidget *icon = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(button_delete), icon); }
      gtk_widget_set_name(button_delete, file_list[i]);
      g_signal_connect(button_delete, "pressed", G_CALLBACK(button_delete_handler), (gpointer) fs);

      GtkWidget *button_hbox;
      button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

      gtk_box_pack_start(GTK_BOX(button_hbox), folder_button, true, true, 0);
      gtk_box_pack_start(GTK_BOX(button_hbox), separator, false, false, 5);

      if (file_io_string_is_file(file_fullpath)){
        GtkWidget *button_edit = gtk_button_new();
        gtk_widget_set_tooltip_text(button_edit, "Edit password");
        { GtkWidget *icon = gtk_image_new_from_icon_name("format-text-direction-ltr", GTK_ICON_SIZE_MENU);
        gtk_button_set_image(GTK_BUTTON(button_edit), icon); }
        gtk_box_pack_start(GTK_BOX(button_hbox), button_edit, false, false, 0);
      }

      gtk_box_pack_start(GTK_BOX(button_hbox), button_rename, false, false, 0);
      gtk_box_pack_end(GTK_BOX(button_hbox), button_delete, false, false, 0);

      gtk_box_pack_start(GTK_BOX(vbox), button_hbox, false, false, 0);
    } else {
      gtk_box_pack_start(GTK_BOX(vbox), folder_button, false, false, 0);
    }
    free((void *) file_fullpath);
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
