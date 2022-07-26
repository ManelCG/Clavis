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

#include <locale.h>
#include <libintl.h>

#if defined(_WIN32) || defined (WIN32)
#include <windows.h>
#endif

#include <clavis_macros.h>

void draw_main_window(GtkWidget *widget, gpointer data){
  printf("Drawing main window!\n");
}

int main(int argc, char *argv[]){
  #if defined(_WIN32) || defined (WIN32)
  //Set Windows Locale environment
  // setlocale(LC_ALL, "ru-ru");
  // SetThreadLocale(1049);
  // setlocale(LC_ALL, "es-Es");
  // SetThreadLocale(1034);
  // setlocale(LC_ALL, "ca");
  // SetThreadLocale(1027);
  #endif

  char localedir[strlen(file_io_get_clavis_folder()) + strlen("locale") + 8];
  #ifdef __unix__
  sprintf(localedir, "%s/%s", file_io_get_clavis_folder(), "locale");
  #elif defined(_WIN32) || defined (WIN32)
  sprintf(localedir, "%s\\%s", file_io_get_clavis_folder(), "locale");
  #endif

  bindtextdomain(CLAVIS_LOCALE_, localedir);
  textdomain(CLAVIS_LOCALE_);

  const char *papath = get_password_store_path();
  if (chdir(papath) != 0){
    mkdir_handler(papath);

    chdir(papath);
  }
  free((char *) papath);

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
