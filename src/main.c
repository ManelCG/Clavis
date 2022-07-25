#include <gtk/gtk.h>

#include <clavis_popup.h>
#include <clavis_normal.h>

#include <clavis_constants.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdbool.h>

void draw_main_window(GtkWidget *widget, gpointer data){
  printf("Drawing main window!\n");
}

int main(int argc, char *argv[]){
  chdir("/home/hrad/.password-store/");
  int clavis_mode = CLAVIS_NORMAL_MODE;
  int opt;
  while ((opt = getopt(argc, argv, ":p")) != -1){
    switch(opt){
      case 'p':
        clavis_mode = CLAVIS_POPUP_MODE;
        return clavis_popup_main(argc-1, &argv[1]);
        break;
      case '?':
        printf("Unknown option.\n");
        break;
    }
  }

  if (clavis_mode == CLAVIS_NORMAL_MODE){
    return clavis_normal_main(argc-1, &argv[1]);
  }
}
