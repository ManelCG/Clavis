#include <gtk/gtk.h>

#include <clavis_popup.h>
#include <clavis_normal.h>

#include <file_io.h>

#include <clavis_constants.h>
#include <gui_templates.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>

void draw_main_window(GtkWidget *widget, gpointer data){
  printf("Drawing main window!\n");
}

int main(int argc, char *argv[]){
  const char *papath = get_password_store_path();
  if (chdir(papath) != 0){
    mkdir_handler(papath);

    chdir(papath);
  }
  free((char *) papath);

  #if defined(_WIN32) || defined (WIN32)
  const char *kspath = get_key_store_path();
  mkdir_handler(kspath);
  free((char *) kspath);
  #endif

  int clavis_mode = CLAVIS_NORMAL_MODE;
  int opt;
  while ((opt = getopt(argc, argv, ":p")) != -1){
    switch(opt){
      case 'p':
        clavis_mode = CLAVIS_POPUP_MODE;
        return clavis_popup_main(argc, argv);
        break;
      case '?':
        printf("Unknown option.\n");
        break;
    }
  }

  if (clavis_mode == CLAVIS_NORMAL_MODE){
    return clavis_normal_main(argc, argv);
  }
}
