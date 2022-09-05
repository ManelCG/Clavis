#include <gui_templates.h>
#include <stdbool.h>
#include <file_io.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include <clavis_constants.h>
#include <clavis_passgen.h>

#include <clavis_popup.h>
#include <clavis_normal.h>

#ifdef __unix__
#include <sys/wait.h>
#elif defined(_WIN32) || defined (WIN32)
#endif

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
      gtk_main_quit();
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
      close(p[0]);
      write(p[1], pw, strlen(pw));
      close(p[1]);

    #elif defined(_WIN32) || defined (WIN32)
      printf("Autotyping in Windows is still WIP!\n");
    #endif
  }
}
void gui_templates_synthesize_button(GtkWidget *w, gpointer data){
  GtkWidget *button = (GtkWidget *) data;
  g_signal_emit_by_name(button, "clicked");
}
void gui_templates_pull_from_repo(){
  #ifdef __unix__
  int p_sync[2];
  int pid;
  if (pipe(p_sync) != 0){
    perror("Could not pipe");
    return;
  }
  pid = fork();
  if (pid < 0){
    perror("Could not fork");
    return;
  }

  if (pid == 0){
    execlp("git", "git", "pull", "--ff", "--no-edit", NULL);
    return;
  }

  wait(NULL);
  close(p_sync[1]);
  char c;
  while(read(p_sync[0], &c, 1)){}
  close(p_sync[0]);
  return;
  #elif defined(_WIN32) || defined (WIN32)

  #endif
}
void gui_templates_push_to_repo(){
  #ifdef __unix__
  int p_sync[2];
  int pid;
  if (pipe(p_sync) != 0){
    perror("Could not pipe");
    return;
  }
  pid = fork();
  if (pid < 0){
    perror("Could not fork");
    return;
  }

  if (pid == 0){
    execlp("git", "git", "push", NULL);
    return;
  }

  wait(NULL);
  close(p_sync[1]);
  char c;
  while(read(p_sync[0], &c, 1)){}
  close(p_sync[0]);
  return;
  #elif defined(_WIN32) || defined (WIN32)

  #endif
}
void gui_templates_sync_repo(){
  gui_templates_pull_from_repo();
  gui_templates_push_to_repo();
}

int gui_templates_git_config_window(){
  GtkWidget *dialog;
  int response;

  const char *user_git_email = NULL;
  const char *user_git_name = NULL;
  const char *user_git_repo = NULL;

  _Bool create_new;

  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Configure your Git server:");

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Apply");
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }

  GtkWindow *window = GTK_WINDOW(dialog);

  gtk_window_set_title(window, "Git Syncronization Config");
  gtk_window_set_resizable(window, false);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_default_size(GTK_WINDOW(window), 0, 0);

  //GTK Widgets
  GtkWidget *dialog_box;

  GtkWidget *main_vbox;
  GtkWidget *name_vbox;
  GtkWidget *mail_vbox;
  GtkWidget *name_email_hbox;
  GtkWidget *lower_buttons_hbox;

  GtkWidget *label_username;
  GtkWidget *label_email;
  GtkWidget *entry_username;
  GtkWidget *entry_email;

  GtkWidget *label_repo_url;
  GtkWidget *entry_repo_url;

  GtkWidget *button_info;
  GtkWidget *button_cancel;
  GtkWidget *button_confirm;

  GtkWidget *toggle_create_new;

  GtkWidget **entry_list = malloc(sizeof(GtkWidget *) * 3);

  //Instance
  //User Config
  label_username = gtk_label_new("Git username");
  label_email = gtk_label_new("Git email");
  entry_username = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Git username");
  entry_email = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Git email");

  user_git_name = file_io_get_git_config_field("user.name");
  user_git_email = file_io_get_git_config_field("user.email");

  if (user_git_name != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_username), user_git_name);
    free((void *) user_git_name);
  }
  if (user_git_email != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_email), user_git_email);
    free((void *) user_git_email);
  }

  name_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  mail_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(name_vbox), label_username, false, false, 0);
  gtk_box_pack_start(GTK_BOX(name_vbox), entry_username, false, false, 0);
  gtk_box_pack_start(GTK_BOX(mail_vbox), label_email, false, false, 0);
  gtk_box_pack_start(GTK_BOX(mail_vbox), entry_email, false, false, 0);

  name_email_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), name_vbox, false, false, 0);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), mail_vbox, false, false, 0);

  //Repo config
  label_repo_url = gtk_label_new("Git repo URL");
  entry_repo_url = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_repo_url), "Git repo URL");

  user_git_repo = file_io_get_git_config_field("remote.origin.url");
  if (user_git_repo != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_repo_url), user_git_repo);
    free((void *) user_git_repo);
  }

  //Buttons
  button_info = gtk_button_new_with_label("Help");
  { GtkWidget *icon = gtk_image_new_from_icon_name("system-help", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_info), icon); }

  button_cancel = gtk_button_new_with_label("Cancel");
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_cancel), icon); }
  g_signal_connect(button_cancel, "activate", G_CALLBACK(destroy), (gpointer) window);
  g_signal_connect(button_cancel, "pressed", G_CALLBACK(destroy), (gpointer) window);

  button_confirm = gtk_button_new_with_label("Confirm");
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_confirm), icon); }

  lower_buttons_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(lower_buttons_hbox), button_info, true, true, 0);
  gtk_box_pack_start(GTK_BOX(lower_buttons_hbox), button_cancel, true, true, 0);
  gtk_box_pack_start(GTK_BOX(lower_buttons_hbox), button_confirm, true, true, 0);

  toggle_create_new = gtk_check_button_new_with_label("Create new repository");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_create_new), true);


  //Main vbox
  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), name_email_hbox, false, false, 0);

  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(main_vbox), separator, false, false, 5);}

  gtk_box_pack_start(GTK_BOX(main_vbox), label_repo_url, false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), entry_repo_url, false, false, 0);

  gtk_box_pack_start(GTK_BOX(main_vbox), toggle_create_new, false, false, 0);

  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(main_vbox), separator, false, false, 5);}

  entry_list[0] = entry_username;
  entry_list[1] = entry_email;
  entry_list[2] = entry_repo_url;

  //Showing window
  dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_box_pack_start(GTK_BOX(dialog_box), main_vbox, false, false, 0);

  gtk_widget_show_all(GTK_WIDGET(window));
  response = gtk_dialog_run(GTK_DIALOG(dialog));

  if (response != GTK_RESPONSE_OK){
    destroy(dialog, dialog);
    return -1;
  }

  user_git_name  = gtk_entry_get_text(GTK_ENTRY(entry_username));
  user_git_email = gtk_entry_get_text(GTK_ENTRY(entry_email));
  user_git_repo  = gtk_entry_get_text(GTK_ENTRY(entry_repo_url));

  if (strcmp(user_git_name, "") == 0){
    user_git_name = NULL;
  }
  if (strcmp(user_git_email, "") == 0){
    user_git_email = NULL;
  }
  if (strcmp(user_git_repo, "") == 0){
    user_git_repo = NULL;
  }

  create_new = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_create_new));

  file_io_init_git_server(user_git_name, user_git_email, user_git_repo, create_new, true);

  destroy(dialog, dialog);
  if (create_new){
    return 0;
  } else {
    return 1;
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
void gui_templates_toggle_widget_visible_handler(GtkWidget *toggle, gpointer data){
  GtkWidget *wid = (GtkWidget *) data;
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle))){
    gtk_widget_show_all(wid);
  } else {
    gtk_widget_hide(wid);
  }
}
void button_newpassword_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;
  passgen *pg = passgen_new();

  GtkWidget *dialog;
  if (strcmp(gtk_widget_get_name(widget), CLAVIS_BUTTON_NEWPASSWORD_NAME) == 0){
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Add new password:");
    gtk_window_set_title(GTK_WINDOW(dialog), "Add new password to Clavis");
  } else {
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Edit existing password:");
    gtk_window_set_title(GTK_WINDOW(dialog), "Edit password in Clavis");
  }
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

  toggle_visibility = gtk_check_button_new_with_label("Display password");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_visibility), true);
  g_signal_connect(toggle_visibility, "toggled", G_CALLBACK(toggle_visibility_handler), (gpointer) entry_password);

  #ifdef __unix__
  button_generate = gtk_button_new();
  #elif defined(_WIN32) || defined (WIN32)
  button_generate = gtk_button_new_with_label("Generate");
  #endif
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
  if (strcmp(gtk_widget_get_name(widget), CLAVIS_BUTTON_NEWPASSWORD_NAME) == 0){
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_passname), "New password name");
    gtk_entry_set_text(GTK_ENTRY(entry_passname), "new_password");
  } else {
    char buffer[strlen(gtk_widget_get_name(widget))];
    strcpy(buffer, gtk_widget_get_name(widget));
    if (strlen(buffer) > 4 && strcmp(&buffer[strlen(buffer)-4], ".gpg") == 0){
      buffer[strlen(buffer)-4] = '\0';
    }
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_passname), "Password name");
    gtk_entry_set_text(GTK_ENTRY(entry_passname), buffer);
    gtk_widget_set_can_focus(entry_passname, false);
    gtk_widget_set_sensitive(entry_passname, false);
  }

  saveto_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(saveto_vbox), label_saveto, false, false, 0);
  gtk_box_pack_start(GTK_BOX(saveto_vbox), entry_passname, false, false, 0);



  //Main left vbox
  GtkWidget *new_pass_label = gtk_label_new("New password:");
  left_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  gtk_box_pack_start(GTK_BOX(left_vbox), saveto_vbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(left_vbox), separator, false, false, 5);}
  gtk_box_pack_start(GTK_BOX(left_vbox), new_pass_label, false, false, 5);
  gtk_box_pack_start(GTK_BOX(left_vbox), password_hbox, false, false, 0);
  gtk_box_pack_start(GTK_BOX(left_vbox), toggle_visibility, false, false, 5);

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

  GtkWidget *dummy_entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(main_hbox), dummy_entry, false, false, 0);

  gtk_box_pack_start(GTK_BOX(main_hbox), left_vbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
  gtk_box_pack_start(GTK_BOX(main_hbox), separator, false, false, 15);}
  gtk_box_pack_start(GTK_BOX(main_hbox), right_vbox, false, false, 0);


  gtk_box_pack_start(GTK_BOX(dialog_box), main_hbox, false, false, 0);
  gtk_widget_show_all(dialog);
  gtk_widget_hide(dummy_entry);


  _Bool valid_password = false;
  while (! valid_password){
    valid_password = true;
    int response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response == GTK_RESPONSE_OK){
      if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_password))) == 0){
        GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, "Please enter a password");
        gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

        GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
        gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Okay");
        gtk_dialog_run(GTK_DIALOG(dialog_failure));
        destroy(dialog_failure, dialog_failure);
        valid_password = false;
      }
      if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_passname))) == 0){
        GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, "Please give a name to your password");
        gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

        GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
        gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Okay");
        gtk_dialog_run(GTK_DIALOG(dialog_failure));
        destroy(dialog_failure, dialog_failure);
        valid_password = false;
      }


      if (valid_password){
        const char *password = gtk_entry_get_text(GTK_ENTRY(entry_password));
        const char *name = folderstate_file_get_full_path_from_string(fs, gtk_entry_get_text(GTK_ENTRY(entry_passname)));
        file_io_encrypt_password(password, name);
      }
    }
  }

  destroy(dialog, dialog);

  folderstate_reload(fs);
  GtkWidget *parent = gtk_widget_get_toplevel(widget);
  draw_main_window_handler(parent, fs);

}
void button_newfolder_handler(GtkWidget *widget, gpointer data){
  folderstate *fs = (folderstate *) data;

  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "New folder name:");
  gtk_window_set_title(GTK_WINDOW(dialog), "New folder");
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
      #ifdef __unix__
      file_io_remove_password(path);
      #elif defined(_WIN32) || defined (WIN32)
      remove(path);
      #endif
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

  draw_main_window_handler(parent, fs);
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

      #ifdef __unix__
      GtkWidget *button_rename = gtk_button_new();
      #elif defined(_WIN32) || defined (WIN32)
      GtkWidget *button_rename = gtk_button_new_with_label("Rename");
      #endif
      gtk_widget_set_tooltip_text(button_rename, "Rename file");
      { GtkWidget *icon = gtk_image_new_from_icon_name("document-edit", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(button_rename), icon); }
      gtk_widget_set_name(button_rename, file_list[i]);
      g_signal_connect(button_rename, "pressed", G_CALLBACK(button_rename_handler), (gpointer) fs);

      #ifdef __unix__
      GtkWidget *button_delete = gtk_button_new();
      #elif defined(_WIN32) || defined (WIN32)
      GtkWidget *button_delete = gtk_button_new_with_label("Delete");
      #endif
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
        #ifdef __unix__
        GtkWidget *button_edit = gtk_button_new();
        { GtkWidget *icon = gtk_image_new_from_icon_name("format-text-direction-ltr", GTK_ICON_SIZE_MENU);
        gtk_button_set_image(GTK_BUTTON(button_edit), icon); }
        #elif defined(_WIN32) || defined (WIN32)
        GtkWidget *button_edit = gtk_button_new_with_label("Edit");
        #endif
        gtk_widget_set_name(button_edit, file_list[i]);
        gtk_widget_set_tooltip_text(button_edit, "Edit password");
        g_signal_connect(button_edit, "pressed", G_CALLBACK(button_newpassword_handler), (gpointer) fs);
        g_signal_connect(button_edit, "activate", G_CALLBACK(button_newpassword_handler), (gpointer) fs);
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

int gui_templates_password_store_init_handler(){
  #ifdef __unix__
  if (!file_io_string_is_file(".gpg-id")){
    GtkWidget *dialog;
    int response;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Hey! It seems this is your first time running Clavis.\nInitialize Password Store?");
    gtk_window_set_title(GTK_WINDOW(dialog), "Welcome to Clavis!");
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
    GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
    GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Accept");
    { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
    // GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));


    gtk_widget_show_all(dialog);
    response = gtk_dialog_run(GTK_DIALOG(dialog));

    if (response != GTK_RESPONSE_OK){
      destroy(dialog, dialog);
      return 1;
    }

    destroy(dialog, dialog);

    int init;

    init = gui_templates_initialize_password_store();

    return init;
  } else {
    return 0;
  }
  #elif defined(_WIN32) || defined (WIN32)
  return 0;
  #endif

}

void gui_templates_fill_combo_box_with_gpg_keys(GtkWidget *combo){
  #ifdef __unix__
  int nkeys;
  char **keys = file_io_get_full_gpg_keys(&nkeys);

  if (keys != NULL){
    for (int i = 0; i < nkeys; i++){
      gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, keys[i]);
      free(keys[i]);
    }
    free(keys);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
  }
  #elif defined(_WIN32) || defined (WIN32)

  #endif
}

void gui_templates_export_key_handler(const char *key, _Bool private){
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Save Keys", NULL, GTK_FILE_CHOOSER_ACTION_SAVE, "_Cancel", GTK_RESPONSE_CANCEL, "_Save", GTK_RESPONSE_ACCEPT, NULL);

  const char *cwd = getcwd(NULL, 0);
  const char *userhome = getenv("HOME");
  chdir(userhome);

  char *keyname = malloc(sizeof(char) * (strlen(key) + 5));
  strcpy(keyname, key);
  if (private){
    strcat(keyname, ".sec");
  } else {
    strcat(keyname, ".pub");
  }

  GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
  gtk_file_chooser_set_do_overwrite_confirmation(chooser, true);
  gtk_file_chooser_set_current_name(chooser, keyname);


  int response = gtk_dialog_run(GTK_DIALOG(dialog));

  if (response == GTK_RESPONSE_ACCEPT){
    GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);

    free(keyname);

    keyname = gtk_file_chooser_get_filename(chooser);

    #ifdef __unix__
    file_io_export_gpg_keys(key, keyname, private);
    #elif defined(_WIN32) || defined (WIN32)

    #endif

    g_free(keyname);
  }
  chdir(cwd);
  destroy(dialog, dialog);
}

void gui_templates_export_key_handler_combobox(GtkWidget *w, gpointer data){
  GtkWidget *combo = (GtkWidget *) data;
  const char *key = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo));

  if (key == NULL){
    return;
  }

  gui_templates_export_key_handler(key, true);
}

void gui_templates_export_key_handler_entry(GtkWidget *w, gpointer data){
  GtkWidget *entry = (GtkWidget *) data;
  const char *key = gtk_entry_get_text(GTK_ENTRY(entry));

  if (key == NULL){
    return;
  }

  gui_templates_export_key_handler(key, true);
}

void gui_templates_export_key_handler_label(GtkWidget *w, gpointer data){
  GtkWidget *label = (GtkWidget *) data;
  const char *key = gtk_label_get_text(GTK_LABEL(label));

  if (key == NULL){
    return;
  }

  gui_templates_export_key_handler(key, true);
}


void gui_templates_import_key_handler(){
  GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File", NULL, GTK_FILE_CHOOSER_ACTION_OPEN, "_Cancel", GTK_RESPONSE_CANCEL, "_Open", GTK_RESPONSE_ACCEPT, NULL);
  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  if (response == GTK_RESPONSE_ACCEPT){
    #ifdef __unix__
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);
    filename = gtk_file_chooser_get_filename (chooser);

    int p_sync[2];
    if (pipe(p_sync) < 0){
      perror("Could not pipe");
    }

    int pid = fork();
    if (pid < 0){
      perror("Could not fork");
    }
    if (pid == 0){  //Child
      close(p_sync[0]);
      execlp("gpg", "gpg", "--import", filename, NULL);
      exit(-1);
    }

    wait(NULL);

    close(p_sync[1]);
    char c;
    while(read(p_sync[0], &c, 1)){
    }
    close(p_sync[0]);
    g_free(filename);

    #elif defined(_WIN32) || defined (WIN32)

    #endif
  }

  gtk_widget_destroy(dialog);
}
int gui_templates_create_key_handler(){
  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Configure your new GPG key:");
  gtk_window_set_title(GTK_WINDOW(dialog), "Clavis Master Key creation");

  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Create key");
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }


  //Boxes
  GtkWidget *main_vbox;
  GtkWidget *keytype_vbox;
  GtkWidget *keylen_vbox;
  GtkWidget *top_hbox;

  GtkWidget *expiration_vbox;
  GtkWidget *expiration_hbox;

  GtkWidget *name_email_hbox;
  GtkWidget *personal_info_vbox;

  GtkWidget *password_vbox;
  GtkWidget *password_hbox;

  //Widgets
  GtkWidget *combo_keytype;
  GtkWidget *entry_keylen;

  GtkWidget *entry_expiration_n;
  GtkWidget *combo_expiration_type;

  GtkWidget *entry_name;
  GtkWidget *entry_email;
  GtkWidget *entry_comment;

  GtkWidget *entry_password_1;
  GtkWidget *entry_password_2;
  GtkWidget *check_password_visibility;

  GtkWidget *show_advanced_config;

  //Pack
  //KEY TYPE VBOX
  combo_keytype = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_keytype), NULL, "RSA and DSA (default)");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_keytype), NULL, "DSA and ElGamal");
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo_keytype), 0);
  GtkWidget *label_keytype = gtk_label_new("Choose key type:");
  keytype_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(keytype_vbox), label_keytype, true, true, 0);
  gtk_box_pack_start(GTK_BOX(keytype_vbox), combo_keytype, true, true, 0);

  //KEY LENGTH VBOX
  entry_keylen = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_keylen), "Key length");
  gtk_entry_set_text(GTK_ENTRY(entry_keylen), "4096");
  gtk_entry_set_width_chars(GTK_ENTRY(entry_keylen), 6);
  GtkWidget *label_keylen = gtk_label_new("Key size:");
  keylen_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(keylen_vbox), label_keylen, true, true, 0);
  gtk_box_pack_start(GTK_BOX(keylen_vbox), entry_keylen, true, true, 0);

  top_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(top_hbox), keytype_vbox, true, true, 0);
  gtk_box_pack_start(GTK_BOX(top_hbox), keylen_vbox, true, true, 0);

  //EXPIRATION
  GtkWidget *label_expiration = gtk_label_new("Set expiration (0 = eternal)");

  entry_expiration_n = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry_expiration_n), "0");

  combo_expiration_type = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, "Days");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, "Weeks");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, "Months");
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, "Years");
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo_expiration_type), 3);

  expiration_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(expiration_hbox), entry_expiration_n, true, true, 0);
  gtk_box_pack_start(GTK_BOX(expiration_hbox), combo_expiration_type, true, true, 0);

  expiration_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(expiration_vbox), label_expiration, false, false, 0);
  gtk_box_pack_start(GTK_BOX(expiration_vbox), expiration_hbox, false, false, 0);


  //PERSONAL INFO
  GtkWidget *label_personal = gtk_label_new("Personal information:");
  entry_name = gtk_entry_new();
  entry_email = gtk_entry_new();
  entry_comment = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_name), "Your name");
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Key name");
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_comment), "Comment (Optional)");

  name_email_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), entry_name, true, true, 0);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), entry_email, true, true, 0);

  personal_info_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(personal_info_vbox), label_personal, true, true, 0);
  gtk_box_pack_start(GTK_BOX(personal_info_vbox), name_email_hbox, true, true, 0);
  gtk_box_pack_start(GTK_BOX(personal_info_vbox), entry_comment, true, true, 0);

  //KEY PASSWORD
  GtkWidget *display_password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  check_password_visibility = gtk_check_button_new_with_label("Display password");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_password_visibility), false);
  gtk_box_pack_start(GTK_BOX(display_password_hbox), check_password_visibility, true, false, 0);
  password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  entry_password_1 = gtk_entry_new();
  gtk_entry_set_visibility(GTK_ENTRY(entry_password_1), false);
  g_signal_connect(check_password_visibility, "toggled", G_CALLBACK(toggle_visibility_handler), (gpointer) entry_password_1);
  gtk_box_pack_start(GTK_BOX(password_hbox), entry_password_1, true, true, 0);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password_1), "Password");

  entry_password_2 = gtk_entry_new();
  gtk_entry_set_visibility(GTK_ENTRY(entry_password_2), false);
  g_signal_connect(check_password_visibility, "toggled", G_CALLBACK(toggle_visibility_handler), (gpointer) entry_password_2);
  gtk_box_pack_start(GTK_BOX(password_hbox), entry_password_2, true, true, 0);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password_2), "Repeat password");

  GtkWidget *password_label = gtk_label_new("Set key's password:");
  password_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(password_vbox), password_label, false, false, 0);
  gtk_box_pack_start(GTK_BOX(password_vbox), password_hbox, false, false, 0);
  gtk_box_pack_start(GTK_BOX(password_vbox), display_password_hbox, false, false, 0);

  //Advanced frame
  show_advanced_config = gtk_check_button_new_with_label("Show advanced configuration");
  GtkWidget *frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *advanced_check_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(advanced_check_hbox), show_advanced_config, true, false, 0);
  g_signal_connect(show_advanced_config, "toggled", G_CALLBACK(gui_templates_toggle_widget_visible_handler), (gpointer) frame_vbox);

  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(frame_vbox), separator, false, false, 5);}
  gtk_box_pack_start(GTK_BOX(frame_vbox), top_hbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(frame_vbox), separator, false, false, 5);}
  gtk_box_pack_start(GTK_BOX(frame_vbox), expiration_vbox, false, false, 0);

  //MAIN VBOX
  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  GtkWidget *dummy_entry = gtk_entry_new();
  gtk_box_pack_start(GTK_BOX(main_vbox), dummy_entry, false, false, 0);

  gtk_box_pack_start(GTK_BOX(main_vbox), personal_info_vbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(main_vbox), separator, false, false, 5);}
  gtk_box_pack_start(GTK_BOX(main_vbox), password_vbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(main_vbox), separator, false, false, 5);}

  gtk_box_pack_start(GTK_BOX(main_vbox), advanced_check_hbox, false, false, 0);

  gtk_box_pack_start(GTK_BOX(main_vbox), frame_vbox, false, false, 0);

  gtk_box_pack_start(GTK_BOX(dialog_box), main_vbox, false, false, 0);
  gtk_widget_show_all(dialog);

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_advanced_config), false);
  gtk_widget_hide(frame_vbox);
  gtk_widget_hide(dummy_entry);

  _Bool valid_key = false;
  while (! valid_key){
    valid_key = true;
    int response = gtk_dialog_run(GTK_DIALOG(dialog));

    //After accepting
    if (response != GTK_RESPONSE_OK){
      destroy(dialog, dialog);
      return -1;
    }

    if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_keylen))) < 3){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, "Key length must be a 3+ digit number");
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Okay");
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_name))) <=4){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, "Name must be at least 5 letters long.");
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Okay");
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_email))) <=4){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, "Key name must be at least 5 letters long");
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Okay");
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_password_1))) == 0){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, "Please enter a password");
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Okay");
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strcmp(gtk_entry_get_text(GTK_ENTRY(entry_password_1)), gtk_entry_get_text(GTK_ENTRY(entry_password_2))) != 0){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, "Your passwords do not match!");
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Okay");
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }
  }
  //Key entered is valid. Proceed

  #ifdef __unix__
  int p[2];
  if (pipe(p) != 0){
    perror("Could not pipe");
    exit(-1);
  }
  int pid = fork();
  if (pid < 0){
    perror("Could not fork");
    exit(pid);
  }

  if (pid == 0){ //Child
    close(0);
    dup(p[0]);
    close(p[0]);
    close(p[1]);

    execlp("gpg", "gpg", "--full-generate-key", "--batch", NULL);
    exit(-1);
  }
  close(p[0]);

  char entry_buffer[128];
  char buffer[1000] = "\0";
  strcat(buffer, "Key-Type: ");

  sprintf(entry_buffer, "%d", gtk_combo_box_get_active(GTK_COMBO_BOX(combo_keytype))+1);
  strcat(buffer, entry_buffer);

  strcat(buffer, "\nKey-Length: ");
  strcat(buffer, gtk_entry_get_text(GTK_ENTRY(entry_keylen)));

  strcat(buffer, "\nName-Real: ");
  strcat(buffer, gtk_entry_get_text(GTK_ENTRY(entry_name)));

  if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_comment))) != 0){
    strcat(buffer, "\nName-Comment: ");
    strcat(buffer, gtk_entry_get_text(GTK_ENTRY(entry_comment)));
  }

  strcat(buffer, "\nName-Email: ");
  strcat(buffer, gtk_entry_get_text(GTK_ENTRY(entry_email)));


  strcat(buffer, "\nExpire-Date: ");
  strcat(buffer, gtk_entry_get_text(GTK_ENTRY(entry_expiration_n)));
  switch(gtk_combo_box_get_active(GTK_COMBO_BOX(combo_expiration_type))){
    case 0:
      strcat(buffer, "d");
      break;
    case 1:
      strcat(buffer, "w");
      break;
    case 2:
      strcat(buffer, "m");
      break;
    case 3:
      strcat(buffer, "y");
      break;
  }

  strcat(buffer, "\nPassphrase: ");
  strcat(buffer, gtk_entry_get_text(GTK_ENTRY(entry_password_1)));
  strcat(buffer, "\n%commit");

  write(p[1], buffer, strlen(buffer));
  close(p[1]);

  waitpid(pid, NULL, 0);
  destroy(dialog, dialog);
  wait(NULL);
  return 0;
  #elif defined(_WIN32) || defined (WIN32)
  destroy(dialog, dialog);
  return 0;
  #endif

}

void button_refresh_keys_handler(GtkWidget *widget, gpointer data){
  GtkWidget *combo = (GtkWidget *) data;
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
  while (gtk_combo_box_get_active(GTK_COMBO_BOX(combo)) != -1){
    gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT(combo), 0);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
  }

  gui_templates_fill_combo_box_with_gpg_keys(combo);
}

void menu_button_export_gpg_handler(GtkWidget *w, gpointer data){
  #ifdef __unix__
  char key_id[512];
  int fd = open(".gpg-id", O_RDONLY);

  if (fd < 0){
    return;
  }

  int len = read(fd, key_id, 512);
  if (len == 0){
    return;
  }

  key_id[len-1] = '\0';

  const char *widname = gtk_widget_get_name(w);
  if (strcmp(widname, CLAVIS_BUTTON_EXPORT_PUBLIC_KEY_NAME) == 0){
    gui_templates_export_key_handler(key_id, false);
  } else if (strcmp(widname, CLAVIS_BUTTON_EXPORT_PRIVATE_KEY_NAME) == 0){
    gui_templates_export_key_handler(key_id, true);
  }
  #endif
}

int gui_templates_initialize_password_store(){
  GtkWidget *dialog;
  int response;

  //Chose key
  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Choose a GPG key, import one or create a new one.");
  gtk_window_set_title(GTK_WINDOW(dialog), "Select your Clavis Master Key");
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), "Confirm selection");
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }

  //Boxes
  GtkWidget *main_vbox;
  GtkWidget *new_key_hbox;
  GtkWidget *combo_hbox;

  //widgets
  GtkWidget *key_combo_box;

  GtkWidget *button_import;
  GtkWidget *button_export;
  GtkWidget *button_create;
  GtkWidget *button_refresh;

  //Git
  GtkWidget *name_vbox;
  GtkWidget *mail_vbox;
  GtkWidget *name_email_hbox;
  GtkWidget *git_frame_vbox;
  GtkWidget *toggle_hbox;

  GtkWidget *label_username;
  GtkWidget *label_email;
  GtkWidget *entry_username;
  GtkWidget *entry_email;

  GtkWidget *label_repo_url;
  GtkWidget *entry_repo_url;

  GtkWidget *toggle_git_server;
  GtkWidget *toggle_create_new;

  const char *user_git_email = NULL;
  const char *user_git_name = NULL;
  const char *user_git_repo = NULL;

  //Instance
  key_combo_box = gtk_combo_box_text_new();

  button_import = gtk_button_new_with_label("Import key");
  { GtkWidget *icon = gtk_image_new_from_icon_name("insert-object", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_import), icon); }
  g_signal_connect(button_import, "pressed", G_CALLBACK(gui_templates_import_key_handler), NULL);
  g_signal_connect(button_import, "activate", G_CALLBACK(gui_templates_import_key_handler), NULL);
  g_signal_connect(button_import, "pressed", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);
  g_signal_connect(button_import, "activate", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);

  button_export = gtk_button_new_with_label("Export key");
  { GtkWidget *icon = gtk_image_new_from_icon_name("document-save-as", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_export), icon); }
  g_signal_connect(button_export, "pressed", G_CALLBACK(gui_templates_export_key_handler_combobox), (gpointer) key_combo_box);
  g_signal_connect(button_export, "activate", G_CALLBACK(gui_templates_export_key_handler_combobox), (gpointer) key_combo_box);

  button_create = gtk_button_new_with_label("Create new key");
  { GtkWidget *icon = gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_create), icon); }
  g_signal_connect(button_create, "pressed", G_CALLBACK(gui_templates_create_key_handler), NULL);
  g_signal_connect(button_create, "activate", G_CALLBACK(gui_templates_create_key_handler), NULL);
  g_signal_connect(button_create, "pressed", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);
  g_signal_connect(button_create, "activate", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);

  //Pack
  gui_templates_fill_combo_box_with_gpg_keys(key_combo_box);
  // gtk_label_set_width_chars(GTK_LABEL(key_combo_box), 5);

  button_refresh = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("view-refresh", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_refresh), icon); }
  g_signal_connect(button_refresh, "pressed", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);
  g_signal_connect(button_refresh, "activate", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);


  combo_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(combo_hbox), key_combo_box, true, true, 0);
  gtk_box_pack_start(GTK_BOX(combo_hbox), button_refresh, false, false, 0);

  new_key_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(new_key_hbox), button_import, true, true, 0);
  gtk_box_pack_start(GTK_BOX(new_key_hbox), button_export, true, true, 0);
  gtk_box_pack_start(GTK_BOX(new_key_hbox), button_create, true, true, 0);

  //Git config
  //User Config
  label_username = gtk_label_new("Git username");
  label_email = gtk_label_new("Git email");
  entry_username = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), "Git username");
  entry_email = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), "Git email");

  user_git_name = file_io_get_git_config_field("user.name");
  user_git_email = file_io_get_git_config_field("user.email");

  if (user_git_name != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_username), user_git_name);
    free((void *) user_git_name);
  }
  if (user_git_email != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_email), user_git_email);
    free((void *) user_git_email);
  }

  name_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  mail_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(name_vbox), label_username, false, false, 0);
  gtk_box_pack_start(GTK_BOX(name_vbox), entry_username, false, false, 0);
  gtk_box_pack_start(GTK_BOX(mail_vbox), label_email, false, false, 0);
  gtk_box_pack_start(GTK_BOX(mail_vbox), entry_email, false, false, 0);

  name_email_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), name_vbox, true, true, 0);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), mail_vbox, true, true, 0);

  //Repo config
  label_repo_url = gtk_label_new("Git repo URL");
  entry_repo_url = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_repo_url), "Git repo URL");

  user_git_repo = file_io_get_git_config_field("remote.origin.url");
  if (user_git_repo != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_repo_url), user_git_repo);
    free((void *) user_git_repo);
  }

  //Toggles
  toggle_git_server = gtk_check_button_new_with_label("Use git server");
  toggle_create_new = gtk_check_button_new_with_label("Create new repo");
  toggle_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(toggle_hbox), toggle_git_server, true, false, 0);
  gtk_box_pack_start(GTK_BOX(toggle_hbox), toggle_create_new, true, false, 0);

  //Git vbox
  git_frame_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(git_frame_vbox), name_email_hbox, false, false, 0);

  gtk_box_pack_start(GTK_BOX(git_frame_vbox), label_repo_url, false, false, 0);
  gtk_box_pack_start(GTK_BOX(git_frame_vbox), entry_repo_url, false, false, 0);

  gtk_box_pack_start(GTK_BOX(git_frame_vbox), toggle_hbox, false, false, 0);


  //Toggle for git
  GtkWidget *show_git_config = gtk_check_button_new_with_label("Git server settings");
  GtkWidget *git_check_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(git_check_hbox), show_git_config, true, false, 0);
  g_signal_connect(show_git_config, "toggled", G_CALLBACK(gui_templates_toggle_widget_visible_handler), (gpointer) git_frame_vbox);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(show_git_config), false);


  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), combo_hbox, false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), new_key_hbox, false, false, 0);

  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(main_vbox), separator, false, false, 5);}

  gtk_box_pack_start(GTK_BOX(main_vbox), git_check_hbox, false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), git_frame_vbox, false, false, 0);

  gtk_box_pack_start(GTK_BOX(dialog_box), main_vbox, true, true, 0);

  gtk_widget_show_all(dialog);

  gtk_widget_hide(git_frame_vbox);

  response = gtk_dialog_run(GTK_DIALOG(dialog));

  if (response != GTK_RESPONSE_OK){
    destroy(dialog, dialog);
    return 1;
  }

  #ifdef __unix__
  char *key = malloc(sizeof(char) * (strlen(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(key_combo_box)))+1));
  strcpy(key, gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(key_combo_box)));

  file_io_gpg_trust_key(key);

  if (! gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_git_server))){
    printf("Here\n");
    file_io_init_password_store(key);
  } else {
    user_git_name  = gtk_entry_get_text(GTK_ENTRY(entry_username));
    user_git_email = gtk_entry_get_text(GTK_ENTRY(entry_email));
    user_git_repo  = gtk_entry_get_text(GTK_ENTRY(entry_repo_url));

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_create_new))){
      file_io_init_password_store(key);
      file_io_init_git_server(user_git_name, user_git_email, user_git_repo, true, false);
    } else {
      mkdir_handler(".");
      file_io_init_git_server(user_git_name, user_git_email, user_git_repo, false, false);
    }
  }
  #elif defined(_WIN32) || defined (WIN32)

  #endif

  destroy(dialog, dialog);

  free(key);
  return 0;
}

void gui_templates_show_password_store_info_window(GtkWidget *w, gpointer data){
  GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

  gtk_window_set_title(window, "Password Store data");
  gtk_window_set_resizable(window, false);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_default_size(GTK_WINDOW(window), 320, 0);

  //Boxes
  GtkWidget *main_vbox;

  //Widgets
  GtkWidget *entry_pass_dir;

  //Packing
  entry_pass_dir = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry_pass_dir), false);
  gtk_entry_set_text(GTK_ENTRY(entry_pass_dir), getcwd(NULL, 0));

  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), gtk_label_new("Password Store directory"), false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), entry_pass_dir, false, false, 0);

  //Showing window
  gtk_container_add(GTK_CONTAINER(window), main_vbox);
  gtk_widget_show_all(GTK_WIDGET(window));
}

void gui_templates_show_about_window(GtkWidget *w, gpointer data){
  GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

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
