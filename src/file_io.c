#include <sys/stat.h>
#include <dirent.h>

#include <stdbool.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <file_io.h>
#include <algorithms.h>

const char *get_password_store_path(){
  int homelen;
  char *path;

  #ifdef __unix__
    char *pa = ".password-store";

    homelen = strlen(getenv("HOME"));
    path = malloc(sizeof(char) * (homelen + strlen(pa) + 8));

    sprintf(path, "%s/%s/", getenv("HOME"), pa);
  #elif defined(_WIN32) || defined (WIN32)
    char *pa = "Clavis_Passwords";

    homelen = strlen(getenv("HOMEPATH"));
    path = malloc(sizeof(char) * (homelen + strlen(pa) + 8));

    sprintf(path, "%s/%s/", getenv("HOMEPATH"), pa);
  #endif
    printf("Path: %s\n", path);
  return path;
}

int mkdir_handler(const char *path){
  #ifdef __unix__
  if (mkdir(path, S_IRWXU) != 0){
    if (errno != EEXIST){
      return -1;
    } else {
      return EEXIST;
    }
  }
  #elif defined(_WIN32) || defined (WIN32)
  if (mkdir(path) != 0){
    if (errno != EEXIST){
      return -1;
    } else {
      return EEXIST;
    }
  }
  #endif

  return 0;
}

char **file_io_folder_get_file_list(const char *folder, int nfiles){
  char **file_vector = malloc(sizeof(char *) * nfiles);

  DIR *d;
  struct dirent *dir;

  int findex = 0;
  int findex_2;

  d = opendir(folder);
  if (d){
    while ((dir = readdir(d)) != NULL){
      if (dir->d_name[0] != '.'){
        char fullpath[strlen(folder) + strlen(dir->d_name + 8)];
        sprintf(fullpath, "%s/%s", folder, dir->d_name);

        if (file_io_string_is_folder(fullpath)){
          file_vector[findex] = malloc(sizeof(char) * (strlen(dir->d_name) + 8));
          strcpy(file_vector[findex], dir->d_name);
          findex++;
        }

      }
    }
    closedir(d);
  }

  quicksort_stringlist(file_vector, 0, findex-1);
  findex_2 = findex;

  d = opendir(folder);
  if (d){
    while ((dir = readdir(d)) != NULL){
      if (dir->d_name[0] != '.'){
        char fullpath[strlen(folder) + strlen(dir->d_name + 8)];
        sprintf(fullpath, "%s/%s", folder, dir->d_name);

        if (file_io_string_is_file(fullpath)){
          file_vector[findex] = malloc(sizeof(char) * (strlen(dir->d_name) + 8));
          strcpy(file_vector[findex], dir->d_name);
          findex++;
        }

      }
    }
    closedir(d);
  }

  quicksort_stringlist(file_vector, findex_2, findex-1);

  return file_vector;
}

int file_io_folder_get_file_n(const char *folder){
  int nfiles = 0;

  DIR *d;
  struct dirent *dir;
  d = opendir(folder);

  if (d){
    while ((dir = readdir(d)) != NULL){
      if (dir->d_name[0] != '.'){
        nfiles ++;
      }
    }

    closedir(d);
  }

  return nfiles;
}

_Bool file_io_string_is_folder(const char *s){
  struct stat pstat;
  stat(s, &pstat);
  return S_ISDIR(pstat.st_mode);
}

_Bool file_io_string_is_file(const char *s){
  struct stat pstat;
  stat(s, &pstat);
  return S_ISREG(pstat.st_mode);
}
