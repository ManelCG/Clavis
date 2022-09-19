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

#include <locale.h>
#include <libintl.h>
#define _(String) gettext(String)

#ifdef __unix__
#include <sys/wait.h>
#elif defined(_WIN32) || defined (WIN32)
#include <windows.h>
#endif

#define GUI_TEMPLATES_BUTTON_WIDTH 85

#ifdef MAKE_INSTALL
  #define ABOUT_PICTURE_PNG "/usr/lib/clavis/assets/inapp_assets/about_picture.png"
#elif defined __unix__
  #define ABOUT_PICTURE_PNG file_io_get_about_picture_png()
#elif defined(_WIN32) || defined (WIN32)
  #define ABOUT_PICTURE_PNG file_io_get_about_picture_png()
#endif


//Tools
void destroy(GtkWidget *w, gpointer data){
  GtkWidget *window = (GtkWidget *) data;
  gui_templates_clear_container(window);
  gtk_widget_destroy(window);
}

void gui_templates_window_set_clavis_icon(GtkWindow *window){
  #ifdef __unix__
  GdkPixbuf *window_icon = gdk_pixbuf_new_from_file(ABOUT_PICTURE_PNG, NULL);
  window_icon = gdk_pixbuf_scale_simple(window_icon, 16, 16, GDK_INTERP_NEAREST);
  gtk_window_set_icon(window, window_icon);
  #endif
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
      const size_t len = strlen(pw) + 1;
      HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
      memcpy(GlobalLock(hMem), pw, len);
      GlobalUnlock(hMem);
      OpenClipboard(0);
      EmptyClipboard();
      SetClipboardData(CF_TEXT, hMem);
      CloseClipboard();
    #endif
  }
}
void gui_templates_message_dialog(const char *message){
  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, message);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
  gtk_dialog_run(GTK_DIALOG(dialog));

  destroy(dialog, dialog);
}
_Bool gui_templates_overwrite_confirmation(const char *path){
  char dialog_prompt[strlen(path) + 32];

  sprintf(dialog_prompt, _("%s already exists. Overwrite?"), path);
  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Replace"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);
  GtkStyleContext *context = gtk_widget_get_style_context(dialog_button_ok);
  gtk_style_context_add_class(context, "destructive-action");

  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
  gtk_widget_add_accelerator(dialog_button_ok, "clicked", accel_group, GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);

  int response = gtk_dialog_run(GTK_DIALOG(dialog));

  destroy(dialog, dialog);

  if (response == GTK_RESPONSE_OK){
    return true;
  } else {
    return false;
  }
}
void type_entry_with_keyboard_handler(GtkWidget *widget, gpointer data){
  GtkWidget *entry = (GtkWidget *) data;
  const char *pw = gtk_entry_get_text(GTK_ENTRY(entry));
  if (strlen(pw) != 0){
    gtk_main_quit();
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
      close(p[0]);
      write(p[1], pw, strlen(pw));
      close(p[1]);

    #elif defined(_WIN32) || defined (WIN32)
      HWND wd;

      wd = GetActiveWindow();
      CloseWindow(wd);
      sleep(2);

      wd = GetActiveWindow();
      SetForegroundWindow(wd);
      SetFocus(wd);

      INPUT vec[strlen(pw)*2];
      for (int i = 0; i < strlen(pw); i++){
        INPUT input = {0};
        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_UNICODE;
        input.ki.wScan = pw[i];

        vec[(i*2)] = input;

        input.ki.dwFlags |= KEYEVENTF_KEYUP;
        vec[(i*2)+1] = input;
      }

      SendInput(strlen(pw)*2, vec, sizeof(INPUT));
    #endif
  }
}
#if defined (_WIN32) || defined (WIN32)
void change_theme_handler(GtkWidget *widget, gpointer data){
  char *appdata = getenv("APPDATA");
  char themefile[strlen(appdata) + strlen("Clavis\\clavis_use_darktheme") + 8];
  sprintf(themefile, "%s\\%s", appdata, "Clavis\\clavis_use_darktheme");

  if (file_io_string_is_file(themefile)){
    int theme = file_io_get_gtk_theme();
    HANDLE hFile;
    hFile = CreateFile(themefile,
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE){
      return;
    }

    char *newtheme = "0\n";
    switch(theme){
      case CLAVIS_THEME_DARK:
        newtheme = "0\n";
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", false, NULL);
        break;
      case CLAVIS_THEME_LIGHT:
        newtheme = "1\n";
        g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", true, NULL);
        break;
    }

    WriteFile(hFile, newtheme, 2, NULL, NULL);
    CloseHandle(hFile);
  } else {
    char *f_ini = file_io_get_gtk_settings_ini_file();

    HANDLE hFile;
    hFile = CreateFile(f_ini,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE){
      return;
    }

    DWORD bread;
    char *buffer = malloc(sizeof(char) * 4096);

    ReadFile(hFile, buffer, 4095, &bread, NULL);
    buffer[bread] = '\0';

    CloseHandle(hFile);

    hFile = CreateFile(f_ini,
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE){
      return;
    }

    unsigned int line_buff_size = 4096;
    char line_buffer[line_buff_size];
    char *token = buffer;
    char *last_token;
    long long int len;

    char *conf = "gtk-application-prefer-dark-theme";

    while (1 && buffer[0] != '\0'){
      last_token = token;
      token = strchr(token, '\n');
      len = token - last_token;

      if (len > 0){
        snprintf(line_buffer, len+1, last_token);
        char *conf_token = strstr(line_buffer, conf);
        if (conf_token != NULL){
          char *value;
          value = strstr(line_buffer, "false");
          if (value != NULL){
            char buffer_write[64];
            sprintf(buffer_write, "%s = true\n", conf);
            WriteFile(hFile, buffer_write, strlen(buffer_write), NULL, NULL);
            g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", true, NULL);
          }
          value = strstr(line_buffer, "true");
          if (value != NULL){
            char buffer_write[64];
            sprintf(buffer_write, "%s = false\n", conf);
            g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", false, NULL);
            WriteFile(hFile, buffer_write, strlen(buffer_write), NULL, NULL);
          }
        } else {
          char buffer_write[strlen(line_buffer)+8];
          sprintf(buffer_write, "%s\n", line_buffer);
          WriteFile(hFile, buffer_write, strlen(buffer_write), NULL, NULL);
        }
      }

      if (token[0] == '\n'){
        token++;
      }
      if (token[0] == '\0'){
        break;
      }
    }

    CloseHandle(hFile);
    free(f_ini);
    free(buffer);
  }

  GtkWidget *dialog;

  ////Reload?
  //dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, "Theme changed. Reload Clavis?");
  //gtk_window_set_title(GTK_WINDOW(dialog), "Clavis Theme Manager");
  //gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  //gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

  //int response = gtk_dialog_run(GTK_DIALOG(dialog));

  //destroy(dialog, dialog);

  //if (response == GTK_RESPONSE_OK){
  //  gtk_main_quit();
  //  PROCESS_INFORMATION piProcInfo;
  //  STARTUPINFO siStartInfo;
  //  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  //  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  //  siStartInfo.cb = sizeof(STARTUPINFO);
  //  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  //  CreateProcessA(file_io_get_clavis_executable(),
  //                 "clavis.exe",
  //                 NULL,
  //                 NULL,
  //                 true,
  //                 0,
  //                 NULL,
  //                 NULL,
  //                 &siStartInfo,
  //                 &piProcInfo);

  //  CloseHandle(piProcInfo.hProcess);
  //  CloseHandle(piProcInfo.hThread);
  //  return;
  //}
  //return;
}
#endif
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
  perform_git_command("git.exe pull --ff --no-edit");
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
  perform_git_command("git.exe push");
  #endif
}
void gui_templates_sync_repo(){
  gui_templates_pull_from_repo();
  gui_templates_push_to_repo();
}

char *gui_templates_ask_for_git_credentials(){
  GtkWidget *dialog;
  int response;

  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("Please provid Git credentials"));

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Confirm"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);

  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
  gtk_widget_add_accelerator(dialog_button_ok, "clicked", accel_group, GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);


  GtkWindow *window = GTK_WINDOW(dialog);
  gtk_window_set_title(window, _("Git credentials"));
  gtk_window_set_resizable(window, false);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_default_size(GTK_WINDOW(window), 0, 0);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *vbox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *vbox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  GtkWidget *label_1 = gtk_label_new(_("Username:"));
  GtkWidget *label_2 = gtk_label_new(_("Password:"));

  GtkWidget *user_entry = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(user_entry), _("Username"));

  GtkWidget *pass_entry = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(pass_entry), _("Password"));

  gtk_box_pack_start(GTK_BOX(vbox1), label_1, true, true, 0);
  gtk_box_pack_start(GTK_BOX(vbox1), user_entry, true, true, 0);

  gtk_box_pack_start(GTK_BOX(vbox2), label_2, true, true, 0);
  gtk_box_pack_start(GTK_BOX(vbox2), pass_entry, true, true, 0);

  gtk_box_pack_start(GTK_BOX(hbox), vbox1, true, true, 0);
  gtk_box_pack_start(GTK_BOX(hbox), vbox2, true, true, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, true, true, 0);

  gtk_box_pack_start(GTK_BOX(dialog_box), vbox, false, false, 0);

  gtk_widget_show_all(GTK_WIDGET(window));
  response = gtk_dialog_run(GTK_DIALOG(dialog));

  if (response != GTK_RESPONSE_OK){
    destroy(dialog, dialog);
    return NULL;
  }

  const char *user = gtk_entry_get_text(GTK_ENTRY(user_entry));
  const char *pass = gtk_entry_get_text(GTK_ENTRY(pass_entry));
  char *credentials = malloc(sizeof(char) * (strlen(user) + strlen(pass) + 3));
  sprintf(credentials, "%s\n%s\n", user, pass);

  destroy(dialog, dialog);
  return credentials;
}

int gui_templates_git_config_window(){
  GtkWidget *dialog;
  int response;

  const char *user_git_email = NULL;
  const char *user_git_name = NULL;
  const char *user_git_repo = NULL;

  _Bool create_new;

  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("Configure your Git server:"));

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Apply"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);

  GtkWindow *window = GTK_WINDOW(dialog);

  gtk_window_set_title(window, _("Git Syncronization Config"));
  gtk_window_set_resizable(window, false);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_default_size(GTK_WINDOW(window), 0, 0);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

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
  label_username = gtk_label_new(_("Git username"));
  label_email = gtk_label_new(_("Git email"));
  entry_username = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), _("Git username"));
  entry_email = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), _("Git email"));

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
  label_repo_url = gtk_label_new(_("Git repo URL"));
  entry_repo_url = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_repo_url), _("Git repo URL"));

  user_git_repo = file_io_get_git_config_field("remote.origin.url");
  if (user_git_repo != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_repo_url), user_git_repo);
    free((void *) user_git_repo);
  }

  //Buttons
  button_info = gtk_button_new_with_label(_("Help"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("system-help", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_info), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(button_info), true);

  button_cancel = gtk_button_new_with_label(_("Cancel"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_cancel), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(button_cancel), true);
  g_signal_connect(button_cancel, "activate", G_CALLBACK(destroy), (gpointer) window);
  g_signal_connect(button_cancel, "pressed", G_CALLBACK(destroy), (gpointer) window);

  button_confirm = gtk_button_new_with_label(_("Confirm"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_confirm), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(button_confirm), true);

  lower_buttons_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(lower_buttons_hbox), button_info, true, true, 0);
  gtk_box_pack_start(GTK_BOX(lower_buttons_hbox), button_cancel, true, true, 0);
  gtk_box_pack_start(GTK_BOX(lower_buttons_hbox), button_confirm, true, true, 0);

  toggle_create_new = gtk_check_button_new_with_label(_("Create new repository"));
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
  printf(_("Decrypting\n"));
}

_Bool entry_filter_keyrelease_handler(GtkWidget *widget, GdkEventKey *event, gpointer data){
  gpointer *srd = (gpointer *) data;
  folderstate *fs = srd[0];
  GtkWidget *scrollbox = srd[1];
  GtkWidget *output = srd[2];

  _Bool editmode = false;;
  if (strcmp(gtk_widget_get_name(scrollbox), CLAVIS_NORMAL_MODE_NAME) == 0){
    editmode = true;
  }

  if (event == NULL){
    return false;
  }
  if (strcmp(gdk_keyval_name(event->keyval), "Up") == 0){
    folderstate_decrease_state(fs);
  } else if (strcmp(gdk_keyval_name(event->keyval), "Down") == 0){
    folderstate_increase_state(fs);
  } else if (strcmp(gdk_keyval_name(event->keyval), "Return") == 0){
    if (folderstate_get_nfiles(fs) > 0){
      const char *name = folderstate_get_files(fs)[folderstate_get_state(fs)];

      gui_templates_folder_button_from_string(output, name, fs);
    }
    return true;
  } else if (strcmp(gdk_keyval_name(event->keyval), "Escape") == 0){
    if (strcmp(folderstate_get_path(fs), ".") != 0){

      button_goup_handler(output, (gpointer)  fs);
      return true;
    }
  } else {
    return false;
  }

  gui_templates_clear_container(scrollbox);
  gui_templates_get_folder_scrollbox(scrollbox, fs, editmode, output);

  return true;
}

void entry_filter_changed_handler(GtkWidget *widget, gpointer data){
  gpointer *srd = (gpointer *) data;
  folderstate *fs = srd[0];
  GtkWidget *scrollbox = srd[1];
  GtkWidget *output = srd[2];

  _Bool editmode = false;;

  if (strcmp(gtk_widget_get_name(scrollbox), CLAVIS_NORMAL_MODE_NAME) == 0){
    editmode = true;
  }

  folderstate_set_filter(fs, gtk_entry_get_text(GTK_ENTRY(widget)));
  folderstate_reload(fs);

  gui_templates_clear_container(scrollbox);
  gui_templates_get_folder_scrollbox(scrollbox, fs, editmode, output);
}
void folderstate_change_state_handler(GtkWidget *widget, gpointer data){
  gpointer *srd = (gpointer *) data;
  folderstate *fs = srd[0];
  GtkWidget *scrollbox = srd[1];
  GtkWidget *output = srd[2];

  if (strcmp(gtk_widget_get_name(widget), CLAVIS_BUTTON_INCREASE_STATE) == 0){
    folderstate_increase_state(fs);
  } else if (strcmp(gtk_widget_get_name(widget), CLAVIS_BUTTON_DECREASE_STATE) == 0){
    folderstate_decrease_state(fs);
  }

  _Bool editmode = false;;

  if (strcmp(gtk_widget_get_name(scrollbox), CLAVIS_NORMAL_MODE_NAME) == 0){
    editmode = true;
  }

  gui_templates_clear_container(scrollbox);
  gui_templates_get_folder_scrollbox(scrollbox, fs, editmode, output);
}
void passgen_button_handler(GtkWidget *widget, gpointer data){
  char *pw = (char *) passgen_generate_new_password((passgen *) data);

  //Clear allocated memory and free
  for (int i = 0; i < strlen(pw); i++){
    pw[i] = '\0';
  }
  free(pw);
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
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("Add new password:"));
    gtk_window_set_title(GTK_WINDOW(dialog), _("Add new password to Clavis"));
  } else {
    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("Edit existing password:"));
    gtk_window_set_title(GTK_WINDOW(dialog), _("Edit password in Clavis"));
  }
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Add password"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);

  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

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
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password), _("Password"));
  passgen_set_output_entry(pg, entry_password);

  toggle_visibility = gtk_check_button_new_with_label(_("Display password"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(toggle_visibility), true);
  g_signal_connect(toggle_visibility, "toggled", G_CALLBACK(toggle_visibility_handler), (gpointer) entry_password);

  button_generate = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_generate), icon); }
  gtk_widget_set_tooltip_text(button_generate, _("Generate password with options"));
  g_signal_connect(button_generate, "clicked", G_CALLBACK(passgen_button_handler), (gpointer) pg);

  password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(password_hbox), entry_password, true, true, 0);
  gtk_box_pack_start(GTK_BOX(password_hbox), button_generate, false, false, 0);

  //Saveto hbox
  label_saveto = gtk_label_new(_("Set password name:"));
  entry_passname = gtk_entry_new();
  if (strcmp(gtk_widget_get_name(widget), CLAVIS_BUTTON_NEWPASSWORD_NAME) == 0){
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_passname), _("New password name"));
    gtk_entry_set_text(GTK_ENTRY(entry_passname), _("new_password"));
  } else {
    char buffer[strlen(gtk_widget_get_name(widget))];
    strcpy(buffer, gtk_widget_get_name(widget));
    if (strlen(buffer) > 4 && strcmp(&buffer[strlen(buffer)-4], ".gpg") == 0){
      buffer[strlen(buffer)-4] = '\0';
    }
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_passname), _("Password name"));
    gtk_entry_set_text(GTK_ENTRY(entry_passname), buffer);
    gtk_widget_set_can_focus(entry_passname, false);
    gtk_widget_set_sensitive(entry_passname, false);
  }

  saveto_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(saveto_vbox), label_saveto, false, false, 0);
  gtk_box_pack_start(GTK_BOX(saveto_vbox), entry_passname, false, false, 0);



  //Main left vbox
  GtkWidget *new_pass_label = gtk_label_new(_("New password:"));
  left_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

  gtk_box_pack_start(GTK_BOX(left_vbox), saveto_vbox, false, false, 0);
  {GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
  gtk_box_pack_start(GTK_BOX(left_vbox), separator, false, false, 5);}
  gtk_box_pack_start(GTK_BOX(left_vbox), new_pass_label, false, false, 5);
  gtk_box_pack_start(GTK_BOX(left_vbox), password_hbox, false, false, 0);
  gtk_box_pack_start(GTK_BOX(left_vbox), toggle_visibility, false, false, 5);

  //Main right vbox
  GtkWidget *label_passlen = gtk_label_new(_("Password length:"));
  entry_passlen = gtk_entry_new();
  check_numerals = gtk_check_button_new_with_label(_("Numerals"));
  check_symbols = gtk_check_button_new_with_label(_("Symbols"));
  check_pron = gtk_check_button_new_with_label(_("Pronounceable"));
  check_upper = gtk_check_button_new_with_label(_("Uppercase"));
  check_lower = gtk_check_button_new_with_label(_("Lowercase"));

  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_passlen), _("Password length"));
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

  GtkWidget *label_autogen = gtk_label_new(_("Password generator:"));
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
        GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, _("Please enter a password"));
        gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

        GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
        gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Okay"));
        gtk_dialog_run(GTK_DIALOG(dialog_failure));
        destroy(dialog_failure, dialog_failure);
        valid_password = false;
      }
      if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_passname))) == 0){
        GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, _("Please give a name to your password"));
        gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

        GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
        gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Okay"));
        gtk_dialog_run(GTK_DIALOG(dialog_failure));
        destroy(dialog_failure, dialog_failure);
        valid_password = false;
      }


      if (valid_password){
        _Bool should_create = true;
        char *password = (char *) gtk_entry_get_text(GTK_ENTRY(entry_password));
        const char *name = folderstate_file_get_full_path_from_string(fs, gtk_entry_get_text(GTK_ENTRY(entry_passname)));

        char extended_name[strlen(name)+5];
        strcpy(extended_name, name);
        strcat(extended_name, ".gpg");

        if (file_io_string_is_file(extended_name) || file_io_string_is_folder(extended_name)){
          should_create = false;

          _Bool response = gui_templates_overwrite_confirmation(extended_name);

          if (response){
            should_create = true;
          }
        }

        if (should_create){
          #ifdef __unix__
          file_io_encrypt_password(password, name);
          #elif defined(_WIN32) || defined (WIN32)
          file_io_encrypt_password(password, extended_name);

          char git_args[strlen(extended_name) + 64];
          sprintf(git_args, "git.exe add %s", extended_name);
          perform_git_command(git_args);
          sprintf(git_args, "git.exe commit -m \"Added password for %s to store\"", name);
          perform_git_command(git_args);
          #endif
        }
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

  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("New folder name:"));
  gtk_window_set_title(GTK_WINDOW(dialog), _("New folder"));
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

  GtkWidget *entry = gtk_entry_new();
  gtk_box_pack_end(GTK_BOX(dialog_box), entry, false, false, 0);

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Create"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("folder-new-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);

  GtkAccelGroup *accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(dialog), accel_group);
  gtk_widget_add_accelerator(dialog_button_ok, "clicked", accel_group, GDK_KEY_Return, 0, GTK_ACCEL_VISIBLE);
  gtk_widget_add_accelerator(dialog_button_cancel, "clicked", accel_group, GDK_KEY_Escape, 0, GTK_ACCEL_VISIBLE);

  gtk_widget_show_all(dialog);

  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  const char *text = gtk_entry_get_text(GTK_ENTRY(entry));

  if (response == GTK_RESPONSE_OK && strcmp(text, "") != 0){
    const char *path = folderstate_file_get_full_path_from_string(fs, text);
    _Bool should_create = true;

    if (file_io_string_is_file(path) || file_io_string_is_folder(path)){
      should_create = false;

      _Bool response = gui_templates_overwrite_confirmation(path);

      if (response){
        if (file_io_rm_rf(path)){
          should_create = true;
        }
      }
    }

    if (should_create){
      if (mkdir_handler(path) != 0){
        char dialog_prompt[strlen(path) + 32];
        sprintf(dialog_prompt, _("Could not overwrite %s."), path);
        gui_templates_message_dialog(dialog_prompt);
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
  sprintf(dialog_prompt, _("Rename %s:\n"), oldpath);
  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

  GtkWidget *entry = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry), name);
  gtk_box_pack_end(GTK_BOX(dialog_box), entry, false, false, 0);

  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Rename"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("document-edit", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);

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

      _Bool response = gui_templates_overwrite_confirmation(path);

      if (response){
        if (file_io_rm_rf(path)){
          should_rename = true;
        }
      }
    }

    if (should_rename){
      if (rename(oldpath, path) != 0){
        char dialog_prompt[strlen(path) + 32];
        sprintf(dialog_prompt, _("Could not overwrite %s."), path);
        gui_templates_message_dialog(dialog_prompt);
      } else {
        #ifdef __unix__
        int pid;
        if ((pid = fork()) < 0){
          perror("Could not fork");
          return;
        }
        if (pid == 0){
          execlp("git", "git", "rm", oldpath, NULL);
          return;
        }
        waitpid(pid, NULL, 0);
        if ((pid = fork()) < 0){
          perror("Could not fork");
          return;
        }
        if (pid == 0){
          execlp("git", "git", "add", path, NULL);
          return;
        }
        waitpid(pid, NULL, 0);

        char args_commit[strlen(path) + strlen(oldpath) + 64];
        sprintf(args_commit, "git commit -m \"Renamed password %s to %s\"", oldpath, path);

        if ((pid = fork()) < 0){
          perror("Could not fork");
          return;
        }
        if (pid == 0){
          execlp("git", "git", "commit", "-m", args_commit, NULL);
          return;
        }
        waitpid(pid, NULL, 0);

        #elif defined(_WIN32) || defined (WIN32)
        char args_rm[strlen(oldpath) + 64];
        sprintf(args_rm, "git.exe rm %s", oldpath);
        perform_git_command(args_rm);

        char args_add[strlen(path)+64];
        sprintf(args_add, "git.exe add %s", path);
        perform_git_command(args_add);

        char args_commit[strlen(path) + strlen(oldpath) + 64];
        sprintf(args_commit, "git.exe commit -m \"Renamed password %s to %s\"", oldpath, path);
        perform_git_command(args_commit);
        #endif
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
    sprintf(dialog_prompt, _("Folder %s contains %d files. Remove anyways?"), path, file_io_folder_get_file_n(path, ""));
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
    gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

    GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Delete"));
    gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);
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
        sprintf(dialog_prompt, _("Error. Could not remove %s"), path);
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
    sprintf(dialog_prompt, _("Remove file '%s'? This cannot be undone!"), path);
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, dialog_prompt);
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);

    GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
    gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

    GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Delete"));
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

      char args_git[strlen(path) + 64];

      sprintf(args_git, "git.exe rm %s", path);
      perform_git_command(args_git);

      sprintf(args_git, "git.exe commit -m \"Removed password %s\"", path);
      perform_git_command(args_git);
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


  int *sigid = ((int *) g_object_get_data(G_OBJECT(window), CLAVIS_SIGNAL_KEYRELEASE_HANDLER_KEYNAV));
  g_signal_handler_disconnect(G_OBJECT(window), *sigid);
  free(sigid);

  if (strcmp(widgetname, CLAVIS_NORMAL_MODE_NAME) == 0){
    clavis_normal_draw_main_window(window, (gpointer) fs);
  } else if (strcmp(widgetname, CLAVIS_POPUP_MODE_NAME) == 0){
    clavis_popup_draw_main_window(window, (gpointer) fs);
  }
}

_Bool gui_templates_folder_button_from_string(GtkWidget *widget, const char *s, folderstate *fs){
  char pressed_fullpath[strlen(folderstate_get_path(fs)) + strlen(s) + 8];
  strcpy(pressed_fullpath, folderstate_get_path(fs));
  strcat(pressed_fullpath, "/");
  strcat(pressed_fullpath, s);

  if (file_io_string_is_folder(pressed_fullpath)){
    folderstate_chdir(fs, s);

    GtkWidget *parent = gtk_widget_get_toplevel(widget);

    draw_main_window_handler(parent, fs);
    return true;
  } else if (file_io_string_is_file(pressed_fullpath)){
    const char *pass = file_io_decrypt_password(pressed_fullpath);
    if (pass != NULL){
      gtk_entry_set_text(GTK_ENTRY(widget), pass);
      free((char *) pass);
    }
  }
  return false;
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
    if (pass != NULL){
      gtk_entry_set_text(GTK_ENTRY(label), pass);
      free((char *) pass);
    }
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

  int selected = folderstate_get_state(fs);

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
      { GtkWidget *icon = gtk_image_new_from_icon_name("folder-symbolic", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(folder_button), icon); }
      gtk_button_set_always_show_image(GTK_BUTTON(folder_button), true);
    } else if (file_io_string_is_file(file_fullpath)){
      g_signal_connect(folder_button, "activate", G_CALLBACK(file_button_handler), (gpointer) output_widget);
      g_signal_connect(folder_button, "pressed", G_CALLBACK(file_button_handler), (gpointer) output_widget);
      files_section = true;
      //Add folder icon to button
      if (strlen(file_fullpath) > 4 && strcmp(&file_fullpath[strlen(file_fullpath)-4], ".gpg") == 0){
        { GtkWidget *icon = gtk_image_new_from_icon_name("channel-secure-symbolic", GTK_ICON_SIZE_MENU);
        gtk_button_set_image(GTK_BUTTON(folder_button), icon); }
        gtk_button_set_always_show_image(GTK_BUTTON(folder_button), true);
      } else {

        { GtkWidget *icon = gtk_image_new_from_icon_name("text-x-generic-symbolic", GTK_ICON_SIZE_MENU);
        gtk_button_set_image(GTK_BUTTON(folder_button), icon); }
        gtk_button_set_always_show_image(GTK_BUTTON(folder_button), true);
      }
    }

    if (selected == i){
      GtkStyleContext *context = gtk_widget_get_style_context(folder_button);
      gtk_style_context_add_class(context, "suggested-action");
    }

    if (files_section && first_file && has_folders){
      first_file = false;
      GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
      gtk_box_pack_start(GTK_BOX(vbox), separator, false, false, 5);
    }

    if (editmode){
      GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_VERTICAL);

      GtkWidget *button_rename = gtk_button_new();
      gtk_widget_set_tooltip_text(button_rename, _("Rename file"));
      { GtkWidget *icon = gtk_image_new_from_icon_name("document-edit", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(button_rename), icon); }
      gtk_button_set_always_show_image(GTK_BUTTON(button_rename), true);
      gtk_widget_set_name(button_rename, file_list[i]);
      g_signal_connect(button_rename, "pressed", G_CALLBACK(button_rename_handler), (gpointer) fs);

      GtkWidget *button_delete = gtk_button_new();
      gtk_widget_set_tooltip_text(button_delete, _("Delete"));
      { GtkWidget *icon = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_MENU);
      gtk_button_set_image(GTK_BUTTON(button_delete), icon); }
      gtk_button_set_always_show_image(GTK_BUTTON(button_delete), true);
      gtk_widget_set_name(button_delete, file_list[i]);
      g_signal_connect(button_delete, "pressed", G_CALLBACK(button_delete_handler), (gpointer) fs);

      GtkWidget *button_hbox;
      button_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

      gtk_box_pack_start(GTK_BOX(button_hbox), folder_button, true, true, 0);
      gtk_box_pack_start(GTK_BOX(button_hbox), separator, false, false, 5);

      if (file_io_string_is_file(file_fullpath)){
        GtkWidget *button_edit = gtk_button_new();
        #ifdef __unix__
        { GtkWidget *icon = gtk_image_new_from_icon_name("format-text-direction-ltr", GTK_ICON_SIZE_MENU);
        #elif defined(_WIN32) || defined (WIN32)
        { GtkWidget *icon = gtk_image_new_from_icon_name("edit-select-symbolic", GTK_ICON_SIZE_MENU);
        #endif
        gtk_button_set_image(GTK_BUTTON(button_edit), icon); }
        gtk_button_set_always_show_image(GTK_BUTTON(button_edit), true);
        gtk_widget_set_name(button_edit, file_list[i]);
        gtk_widget_set_tooltip_text(button_edit, _("Edit password"));
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
  if (!file_io_string_is_file(".gpg-id")){
    GtkWidget *dialog;
    int response;

    dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("Hey! It seems this is your first time running Clavis.\nInitialize Password Store?"));
    gtk_window_set_title(GTK_WINDOW(dialog), _("Welcome to Clavis!"));
    gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);
    GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
    { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
    gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

    GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
    gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Accept"));
    { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
    gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
    gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);
    gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
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
}

void gui_templates_fill_combo_box_with_gpg_keys(GtkWidget *combo){
  int nkeys;
  char **keys = file_io_get_full_gpg_keys(&nkeys);

  if (keys != NULL && nkeys != 0){
    for (int i = 0; i < nkeys; i++){
      gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo), NULL, keys[i]);
      free(keys[i]);
    }
    free(keys);
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);
  }
}

void gui_templates_export_key_handler(const char *key, _Bool private){
  #ifdef __unix__
  GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Save Keys"), NULL, GTK_FILE_CHOOSER_ACTION_SAVE, _("_Cancel"), GTK_RESPONSE_CANCEL, _("_Save"), GTK_RESPONSE_ACCEPT, NULL);

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

    file_io_export_gpg_keys(key, keyname, private);

    g_free(keyname);
  }
  chdir(cwd);
  destroy(dialog, dialog);
  #elif defined(_WIN32) || defined (WIN32)
  const char *cwd = _getcwd(NULL, 0);

  char filename[MAX_PATH];
  char *token;
  strcpy(filename, key);

  token = strchr(filename, '<');
  if (token[0] == '<'){
    token[0] = '[';
  }
  token = strchr(filename, '>');
  if (token[0] == '>'){
    token[0] = ']';
  }

  OPENFILENAMEA ofn;
  memset(&ofn, 0, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.hInstance = NULL;
  ofn.lpstrFilter = "GPG key\0*.sec;*.pub;*.gpg;*.key\0\0";
  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrTitle = _("Save your GPG key");
  ofn.Flags = OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST;

  if (!GetSaveFileName(&ofn)){
    chdir(cwd);
    free((char *) cwd);
    return;
  }
  chdir(cwd);

  if (private){
    strcat(filename, ".sec");
  } else {
    strcat(filename, ".pub");
  }


  file_io_export_gpg_keys(key, filename, private);

  free((char *) cwd);
  #endif
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
  #ifdef __unix__
  GtkWidget *dialog = gtk_file_chooser_dialog_new(_("Open File"), NULL, GTK_FILE_CHOOSER_ACTION_OPEN, _("_Cancel"), GTK_RESPONSE_CANCEL, _("_Open"), GTK_RESPONSE_ACCEPT, NULL);
  int response = gtk_dialog_run(GTK_DIALOG(dialog));
  if (response == GTK_RESPONSE_ACCEPT){
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

  }

  gtk_widget_destroy(dialog);
  #elif defined(_WIN32) || defined (WIN32)
  const char *cwd = _getcwd(NULL, 0);

  char filename[MAX_PATH];
  filename[0] = '\0';
  OPENFILENAMEA ofn;
  memset(&ofn, 0, sizeof(ofn));
  ofn.lStructSize = sizeof(ofn);
  ofn.hwndOwner = NULL;
  ofn.hInstance = NULL;
  ofn.lpstrFilter = "GPG Keys (.gpg, .sec, .pub, .key)\0*.sec;*.gpg;*.pub;*.key\0\0";
  ofn.lpstrFile = filename;
  ofn.nMaxFile = MAX_PATH;
  ofn.lpstrTitle = _("Select your GPG key");
  ofn.Flags = OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST;

  if (!GetOpenFileName(&ofn)){
    chdir(cwd);
    free((char *) cwd);
    return;
  }
  chdir(cwd);

  HANDLE child_SYNC_rd = NULL;
  HANDLE child_SYNC_wr = NULL;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = true;
  saAttr.lpSecurityDescriptor = NULL;

  CreatePipe(&child_SYNC_rd, &child_SYNC_wr, &saAttr, 0);
  SetHandleInformation(child_SYNC_rd, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = child_SYNC_wr;
  siStartInfo.hStdInput = NULL;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  char gpg_parms[strlen(filename) + 64];
  sprintf(gpg_parms, "gpg.exe --import \"%s\"", filename);

  CreateProcessA("C:\\Program Files (x86)\\GnuPG\\bin\\gpg.exe",
                 gpg_parms,
                 NULL,
                 NULL,
                 true,
                 CREATE_NO_WINDOW,
                 NULL,
                 NULL,
                 &siStartInfo,
                 &piProcInfo);

  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread);
  CloseHandle(child_SYNC_wr);

  char blackhole;
  while(ReadFile(child_SYNC_rd, &blackhole, 1, NULL, NULL)){
  }
  CloseHandle(child_SYNC_rd);

  free((char *) cwd);
  #endif
}
int gui_templates_create_key_handler(){
  GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("Configure your new GPG key:"));
  gtk_window_set_title(GTK_WINDOW(dialog), _("Clavis Master Key creation"));

  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Create key"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);


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
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_keytype), NULL, _("RSA and DSA (default)"));
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_keytype), NULL, _("DSA and ElGamal"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo_keytype), 0);
  GtkWidget *label_keytype = gtk_label_new(_("Choose key type:"));
  keytype_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(keytype_vbox), label_keytype, true, true, 0);
  gtk_box_pack_start(GTK_BOX(keytype_vbox), combo_keytype, true, true, 0);

  //KEY LENGTH VBOX
  entry_keylen = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_keylen), _("Key length"));
  gtk_entry_set_text(GTK_ENTRY(entry_keylen), "4096");
  gtk_entry_set_width_chars(GTK_ENTRY(entry_keylen), 6);
  GtkWidget *label_keylen = gtk_label_new(_("Key size:"));
  keylen_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(keylen_vbox), label_keylen, true, true, 0);
  gtk_box_pack_start(GTK_BOX(keylen_vbox), entry_keylen, true, true, 0);

  top_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(top_hbox), keytype_vbox, true, true, 0);
  gtk_box_pack_start(GTK_BOX(top_hbox), keylen_vbox, true, true, 0);

  //EXPIRATION
  GtkWidget *label_expiration = gtk_label_new(_("Set expiration (0 = eternal)"));

  entry_expiration_n = gtk_entry_new();
  gtk_entry_set_text(GTK_ENTRY(entry_expiration_n), "0");

  combo_expiration_type = gtk_combo_box_text_new();
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, _("Days"));
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, _("Weeks"));
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, _("Months"));
  gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(combo_expiration_type), NULL, _("Years"));
  gtk_combo_box_set_active(GTK_COMBO_BOX(combo_expiration_type), 3);

  expiration_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(expiration_hbox), entry_expiration_n, true, true, 0);
  gtk_box_pack_start(GTK_BOX(expiration_hbox), combo_expiration_type, true, true, 0);

  expiration_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(expiration_vbox), label_expiration, false, false, 0);
  gtk_box_pack_start(GTK_BOX(expiration_vbox), expiration_hbox, false, false, 0);


  //PERSONAL INFO
  GtkWidget *label_personal = gtk_label_new(_("Personal information:"));
  entry_name = gtk_entry_new();
  entry_email = gtk_entry_new();
  entry_comment = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_name), _("Your name"));
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), _("Key name"));
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_comment), _("Comment (Optional)"));

  name_email_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), entry_name, true, true, 0);
  gtk_box_pack_start(GTK_BOX(name_email_hbox), entry_email, true, true, 0);

  personal_info_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(personal_info_vbox), label_personal, true, true, 0);
  gtk_box_pack_start(GTK_BOX(personal_info_vbox), name_email_hbox, true, true, 0);
  gtk_box_pack_start(GTK_BOX(personal_info_vbox), entry_comment, true, true, 0);

  //KEY PASSWORD
  GtkWidget *display_password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
  check_password_visibility = gtk_check_button_new_with_label(_("Display password"));
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_password_visibility), false);
  gtk_box_pack_start(GTK_BOX(display_password_hbox), check_password_visibility, true, false, 0);
  password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  entry_password_1 = gtk_entry_new();
  gtk_entry_set_visibility(GTK_ENTRY(entry_password_1), false);
  g_signal_connect(check_password_visibility, "toggled", G_CALLBACK(toggle_visibility_handler), (gpointer) entry_password_1);
  gtk_box_pack_start(GTK_BOX(password_hbox), entry_password_1, true, true, 0);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password_1), _("Password"));

  entry_password_2 = gtk_entry_new();
  gtk_entry_set_visibility(GTK_ENTRY(entry_password_2), false);
  g_signal_connect(check_password_visibility, "toggled", G_CALLBACK(toggle_visibility_handler), (gpointer) entry_password_2);
  gtk_box_pack_start(GTK_BOX(password_hbox), entry_password_2, true, true, 0);
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_password_2), _("Repeat password"));

  GtkWidget *password_label = gtk_label_new(_("Set key's password:"));
  password_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(password_vbox), password_label, false, false, 0);
  gtk_box_pack_start(GTK_BOX(password_vbox), password_hbox, false, false, 0);
  gtk_box_pack_start(GTK_BOX(password_vbox), display_password_hbox, false, false, 0);

  //Advanced frame
  show_advanced_config = gtk_check_button_new_with_label(_("Show advanced configuration"));
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
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, _("Key length must be a 3+ digit number"));
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Okay"));
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_name))) <=4){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, _("Name must be at least 5 letters long."));
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Okay"));
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_email))) <=4){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, _("Key name must be at least 5 letters long"));
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Okay"));
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strlen(gtk_entry_get_text(GTK_ENTRY(entry_password_1))) == 0){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, _("Please enter a password"));
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Okay"));
      gtk_dialog_run(GTK_DIALOG(dialog_failure));
      destroy(dialog_failure, dialog_failure);
      valid_key = false;
    }

    if (strcmp(gtk_entry_get_text(GTK_ENTRY(entry_password_1)), gtk_entry_get_text(GTK_ENTRY(entry_password_2))) != 0){
      GtkWidget *dialog_failure = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK, _("Your passwords do not match!"));
      gtk_container_set_border_width(GTK_CONTAINER(dialog_failure), 10);

      GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog_failure), GTK_RESPONSE_OK);
      gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Okay"));
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
  #endif

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

  #ifdef __unix__
  strcat(buffer, "\n%commit");
  #endif

  #ifdef __unix__
  write(p[1], buffer, strlen(buffer));
  close(p[1]);

  waitpid(pid, NULL, 0);
  destroy(dialog, dialog);
  wait(NULL);
  return 0;
  #elif defined(_WIN32) || defined (WIN32)
  HANDLE child_IN_rd = NULL;
  HANDLE child_IN_wr = NULL;

  HANDLE child_SYNC_rd = NULL;
  HANDLE child_SYNC_wr = NULL;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = true;
  saAttr.lpSecurityDescriptor = NULL;

  CreatePipe(&child_IN_rd, &child_IN_wr, &saAttr, 0);
  SetHandleInformation(child_IN_wr, HANDLE_FLAG_INHERIT, 0);

  CreatePipe(&child_SYNC_rd, &child_SYNC_wr, &saAttr, 0);
  SetHandleInformation(child_SYNC_rd, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = child_SYNC_wr;
  siStartInfo.hStdInput = child_IN_rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  CreateProcessA("C:\\Program Files (x86)\\GnuPG\\bin\\gpg.exe",
                 "gpg.exe --full-generate-key --batch",
                 NULL,
                 NULL,
                 true,
                 CREATE_NO_WINDOW,
                 NULL,
                 NULL,
                 &siStartInfo,
                 &piProcInfo);

  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread);
  CloseHandle(child_IN_rd);
  CloseHandle(child_SYNC_wr);

  WriteFile(child_IN_wr, buffer, strlen(buffer), NULL, NULL);
  CloseHandle(child_IN_wr);

  char blackhole;
  while(ReadFile(child_SYNC_rd, &blackhole, 1, NULL, NULL)){
  }
  CloseHandle(child_SYNC_rd);

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
  char *gpgid = file_io_get_gpgid();
  if (gpgid == NULL){
    return;
  }

  const char *widname = gtk_widget_get_name(w);
  if (strcmp(widname, CLAVIS_BUTTON_EXPORT_PUBLIC_KEY_NAME) == 0){
    gui_templates_export_key_handler(gpgid, false);
  } else if (strcmp(widname, CLAVIS_BUTTON_EXPORT_PRIVATE_KEY_NAME) == 0){
    gui_templates_export_key_handler(gpgid, true);
  }

  free(gpgid);
}

int gui_templates_initialize_password_store(){
  GtkWidget *dialog;
  int response;

  //Chose key
  dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_BUTTONS_OK_CANCEL, _("Choose a GPG key, import one or create a new one."));
  gtk_window_set_title(GTK_WINDOW(dialog), _("Select your Clavis Master Key"));
  gtk_window_set_position(GTK_WINDOW(dialog), GTK_WIN_POS_CENTER);
  gtk_container_set_border_width(GTK_CONTAINER(dialog), 10);

  GtkWidget *dialog_box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
  GtkWidget *dialog_button_cancel = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
  { GtkWidget *icon = gtk_image_new_from_icon_name("window-close", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_cancel), icon); }
  GtkWidget *dialog_button_ok = gtk_dialog_get_widget_for_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);
  gtk_button_set_label(GTK_BUTTON(dialog_button_ok), _("Confirm selection"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("emblem-ok-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(dialog_button_ok), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_ok), true);
  gtk_button_set_always_show_image(GTK_BUTTON(dialog_button_cancel), true);

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

  button_import = gtk_button_new_with_label(_("Import key"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("insert-object-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_import), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(button_import), true);
  g_signal_connect(button_import, "pressed", G_CALLBACK(gui_templates_import_key_handler), NULL);
  g_signal_connect(button_import, "activate", G_CALLBACK(gui_templates_import_key_handler), NULL);
  g_signal_connect(button_import, "pressed", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);
  g_signal_connect(button_import, "activate", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);

  button_export = gtk_button_new_with_label(_("Export key"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("document-save-as-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_export), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(button_export), true);
  g_signal_connect(button_export, "pressed", G_CALLBACK(gui_templates_export_key_handler_combobox), (gpointer) key_combo_box);
  g_signal_connect(button_export, "activate", G_CALLBACK(gui_templates_export_key_handler_combobox), (gpointer) key_combo_box);

  button_create = gtk_button_new_with_label(_("Create new key"));
  { GtkWidget *icon = gtk_image_new_from_icon_name("document-new-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_create), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(button_create), true);
  g_signal_connect(button_create, "pressed", G_CALLBACK(gui_templates_create_key_handler), NULL);
  g_signal_connect(button_create, "activate", G_CALLBACK(gui_templates_create_key_handler), NULL);
  g_signal_connect(button_create, "pressed", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);
  g_signal_connect(button_create, "activate", G_CALLBACK(button_refresh_keys_handler), (gpointer) key_combo_box);

  //Pack
  gui_templates_fill_combo_box_with_gpg_keys(key_combo_box);
  // gtk_label_set_width_chars(GTK_LABEL(key_combo_box), 5);

  button_refresh = gtk_button_new();
  { GtkWidget *icon = gtk_image_new_from_icon_name("view-refresh-symbolic", GTK_ICON_SIZE_MENU);
  gtk_button_set_image(GTK_BUTTON(button_refresh), icon); }
  gtk_button_set_always_show_image(GTK_BUTTON(button_refresh), true);
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
  label_username = gtk_label_new(_("Git username"));
  label_email = gtk_label_new(_("Git email"));
  entry_username = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_username), _("Git username"));
  entry_email = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_email), _("Git email"));

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
  label_repo_url = gtk_label_new(_("Git repo URL"));
  entry_repo_url = gtk_entry_new();
  gtk_entry_set_placeholder_text(GTK_ENTRY(entry_repo_url), _("Git repo URL"));

  user_git_repo = file_io_get_git_config_field("remote.origin.url");
  if (user_git_repo != NULL){
    gtk_entry_set_text(GTK_ENTRY(entry_repo_url), user_git_repo);
    free((void *) user_git_repo);
  }

  //Toggles
  toggle_git_server = gtk_check_button_new_with_label(_("Use git server"));
  toggle_create_new = gtk_check_button_new_with_label(_("Create new repo"));
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
  GtkWidget *show_git_config = gtk_check_button_new_with_label(_("Git server settings"));
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

  // #ifdef __unix__

  char *key = malloc(sizeof(char) * (strlen(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(key_combo_box)))+1));
  strcpy(key, gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(key_combo_box)));

  file_io_gpg_trust_key(key);

  if (! gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggle_git_server))){
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

  free(key);

  // #elif defined(_WIN32) || defined (WIN32)
  // char *key = malloc(sizeof(char) * (strlen(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(key_combo_box)))+1));
  // strcpy(key, gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(key_combo_box)));

  // printf("Initializing for %s\n", key);
  // #endif

  destroy(dialog, dialog);
  return 0;
}

void gui_templates_show_password_store_info_window(GtkWidget *w, gpointer data){
  GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

  gtk_window_set_title(window, _("Password Store data"));
  gtk_window_set_resizable(window, false);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_default_size(GTK_WINDOW(window), 320, 0);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  //Boxes
  GtkWidget *main_vbox;

  //Widgets
  GtkWidget *entry_pass_dir;

  //Packing
  entry_pass_dir = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry_pass_dir), false);
  gtk_entry_set_text(GTK_ENTRY(entry_pass_dir), getcwd(NULL, 0));

  main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  gtk_box_pack_start(GTK_BOX(main_vbox), gtk_label_new(_("Password Store directory")), false, false, 0);
  gtk_box_pack_start(GTK_BOX(main_vbox), entry_pass_dir, false, false, 0);

  //Showing window
  gtk_container_add(GTK_CONTAINER(window), main_vbox);
  gtk_widget_show_all(GTK_WIDGET(window));
}

GtkWidget *gui_templates_get_language_credits_hbox(const char *text_translationby, const char *language, const char *credits[], const char *webs[], int ncredits){
  char translation_text[strlen(text_translationby) + strlen(language) + 8];
  sprintf(translation_text, text_translationby, language);
  GtkWidget *translation_label = gtk_label_new(translation_text);
  gtk_label_set_xalign(GTK_LABEL(translation_label), 1);

  GtkWidget *left_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *right_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

  gtk_box_pack_start(GTK_BOX(left_vbox), translation_label, 0, true, true);

  for (int i = 0; i < ncredits; i++){
    const char *labeltext;
    GtkWidget *label;
    if (webs[i] == NULL){
      labeltext = credits[i];
      label = gtk_label_new(labeltext);
    } else {
      char *html = malloc(strlen(webs[i]) + strlen(credits[i]) + 32);
      sprintf(html, "<a href=\"%s\">%s</a>", webs[i], credits[i]);
      labeltext = html;

      label = gtk_label_new(NULL);
      gtk_label_set_markup(GTK_LABEL(label), labeltext);
    }

    gtk_label_set_xalign(GTK_LABEL(label), 0);
    gtk_box_pack_start(GTK_BOX(right_vbox), label, 0, true, true);
  }

  gtk_box_pack_start(GTK_BOX(hbox), left_vbox, 0, true, true);
  gtk_box_pack_start(GTK_BOX(hbox), right_vbox, 0, true, true);

  return hbox;
}

void gui_templates_window_credits(GtkWidget *w, gpointer data){
  GtkWindow *wdw = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
  gtk_window_set_title(wdw, _("Clavis acknowledgements"));
  gtk_window_set_resizable(wdw, false);
  gtk_container_set_border_width(GTK_CONTAINER(wdw), 10);
  gtk_window_set_default_size(GTK_WINDOW(wdw), 420, 0);

  gtk_window_set_position(GTK_WINDOW(wdw), GTK_WIN_POS_CENTER);

  const char *text_translationby = _("%s translation by:");

  //Languages:
  const char *spanish = _("Spanish");
  const char *english = _("English");
  const char *catalan = _("Catalan");
  const char *russian = _("Russian");

  //Boxes
  GtkWidget *main_vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

  {
    GtkWidget *label_app_name = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label_app_name), _("<b>Clavis</b>"));
    gtk_box_pack_start(GTK_BOX(main_vbox), label_app_name, 0, true, true);
  }

  //Translation Credits
  {
    const char *language = spanish;
    const char *credits[] = {"Manel Castillo Giménez"};
    const char *webs[] = {"https://github.com/ManelCG"};
    GtkWidget *box = gui_templates_get_language_credits_hbox(text_translationby, language, credits, webs, 1);
    gtk_box_pack_start(GTK_BOX(main_vbox), box, 0, true, true);
  }
  {
    const char *language = russian;
    const char *credits[] = {"Polina Chainikova"};
    const char *webs[] = {"https://github.com/polinatea"};
    GtkWidget *box = gui_templates_get_language_credits_hbox(text_translationby, language, credits, webs, 1);
    gtk_box_pack_start(GTK_BOX(main_vbox), box, 0, true, true);
  }

  gtk_container_add(GTK_CONTAINER(wdw), main_vbox);
  gtk_widget_show_all(GTK_WIDGET(wdw));

}

void gui_templates_show_about_window(GtkWidget *w, gpointer data){
  GtkWindow *window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));

  gtk_window_set_title(window, _("About Clavis"));
  gtk_window_set_resizable(window, false);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);
  gtk_window_set_default_size(GTK_WINDOW(window), 420, 0);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

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
  gtk_label_set_markup(GTK_LABEL(label_app_name), _("<b>Clavis</b>"));

  label_version = gtk_label_new(CLAVIS_VERSION);

  label_description = gtk_label_new(_("Clavis is an easy to use Password Manager."));
  gtk_label_set_justify(GTK_LABEL(label_description),GTK_JUSTIFY_CENTER);
  gtk_label_set_line_wrap(GTK_LABEL(label_description), true);
  gtk_label_set_max_width_chars(GTK_LABEL(label_description), 50);
  gtk_label_set_width_chars(GTK_LABEL(label_description), 50);

  const char *label_git_repo_text = _("<a href=\"https://github.com/ManelCG/clavis\"> Github repository</a>");
  label_git_repo = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label_git_repo), label_git_repo_text);
  gtk_widget_set_tooltip_text(label_git_repo, _("Go to the Clavis Github repository"));

  label_copyright_manel = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label_copyright_manel), _("<small>Copyright © Manel Castillo Giménez</small>"));

  button_close = gtk_button_new_with_label(_("Close"));
  gtk_widget_set_size_request(button_close, GUI_TEMPLATES_BUTTON_WIDTH, 0);
  g_signal_connect(button_close, "clicked", G_CALLBACK(destroy), (gpointer) window);

  button_credits = gtk_button_new_with_label(_("Credits"));
  gtk_widget_set_size_request(button_credits, GUI_TEMPLATES_BUTTON_WIDTH, 0);
  g_signal_connect(GTK_BUTTON(button_credits), "pressed", G_CALLBACK(gui_templates_window_credits), NULL);

  button_license = gtk_button_new_with_label(_("License"));
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
