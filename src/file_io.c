#include <sys/stat.h>
#include <dirent.h>

#include <stdbool.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include <sys/wait.h>

#include <file_io.h>
#include <algorithms.h>
#include <clavis_constants.h>

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

_Bool file_io_rm_rf(const char *path){
  _Bool removed = false;
  if (file_io_string_is_file(path)){
    if (remove(path) == 0){
      removed = true;
    }
  } else if (file_io_string_is_folder(path)){
    if (rmdir(path) != 0){
      int nfiles = file_io_folder_get_file_n(path, "");
      char **files = file_io_folder_get_file_list(path, nfiles, "");

      for (int i = 0; i < nfiles; i++){
        char filepath[strlen(path) + strlen(files[i]) + 8];
        sprintf(filepath, "%s/%s", path, files[i]);
        if (file_io_string_is_file(filepath)){
          if (remove(filepath) == 0){
            removed = true;
          }
        } else if (file_io_string_is_folder(filepath)){
          if (file_io_rm_rf(filepath)){
            removed = true;
          }
        }

        free(files[i]);
      }

      free(files);
      rmdir(path);
    } else {
      removed = true;
    }
  }

  return removed;
}

int file_io_encrypt_password(const char *password, const char *path){
  #ifdef __unix__
    int pid;
    int p[2];
    if (pipe(p) != 0){
      perror("Could not pipe");
      return -1;
    }

    pid = fork();
    if (pid < 0){
      perror("Could not fork");
      return pid;
    }

    if (pid == 0){  //Child
      close(0);
      dup(p[0]);
      close(p[0]);
      close(p[1]);

      execlp("pass", "pass", "add", path, NULL);
      exit(-1);
    }

    close(p[0]);
    write(p[1], password, strlen(password));
    write(p[1], "\n", 1);
    write(p[1], password, strlen(password));
    write(p[1], "\n", 1);
    close(p[1]);

    wait(NULL);

    return 0;
  #elif defined(_WIN32) || defined (WIN32)
    printf("Password encryption is still WIP for Windows.\n");
  #endif
}

const char *file_io_decrypt_password(const char *file){
  #ifdef __unix__
    int pid;
    int p[2];
    if (pipe(p) != 0){
      perror("Could not pipe");
      exit(-1);
    }

    pid = fork();
    if (pid < 0){
      perror("Could not fork");
      exit(pid);
    }

    if (pid == 0){  //Child
      close(1);
      dup(p[1]);
      close(p[0]);
      close(p[1]);

      char password_path[strlen(file) + 32];
      strcpy(password_path, file);
      if (strcmp(&password_path[strlen(password_path)-4], ".gpg") == 0){
        password_path[strlen(password_path)-4] = '\0';
      }
      execlp("pass", "pass", password_path, NULL);
    }
    close(p[1]);
    wait(NULL);

    //Parent
    size_t passlen = DEFAULT_PASSWORD_SIZE;
    int index = 0;
    char *decrypted_password = malloc(sizeof(char) * passlen);
    char c;

    while (read(p[0], &c, sizeof(c))){
      if (c != '\0' && c != '\n'){
        if (index == passlen){
          passlen *= 2;
          decrypted_password = realloc(decrypted_password, passlen);
        }

        decrypted_password[index] = c;
        index++;
      } else {
        break;
      }
    }

    if (index == passlen){
      passlen += 2;
      decrypted_password = realloc(decrypted_password, passlen);
    }
    decrypted_password[index] = '\0';

    return decrypted_password;


  #elif defined(_WIN32) || defined (WIN32)
    printf("Decryption in Windows is still WIP\n");
  #endif
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

char **file_io_folder_get_file_list(const char *folder, int nfiles, const char *filter){
  char **file_vector = calloc(sizeof(char *) * nfiles, 1);

  DIR *d;
  struct dirent *dir;

  int findex = 0;
  int findex_2;

  d = opendir(folder);
  if (d){
    while ((dir = readdir(d)) != NULL){
      if (dir->d_name[0] != '.'){
        char fullpath[strlen(folder) + strlen(dir->d_name) +8];
        sprintf(fullpath, "%s/%s", folder, dir->d_name);

        if (file_io_string_is_folder(fullpath)){
          if (filter == NULL || strcmp(filter, "") == 0 || strstr(dir->d_name, filter) != NULL){
            file_vector[findex] = calloc(sizeof(char) * (strlen(dir->d_name) + 8), 1);
            strcpy(file_vector[findex], dir->d_name);
            findex++;
          }
        }
      }
    }
    closedir(d);
  }

  if (findex > 1){
    quicksort_stringlist(file_vector, 0, findex-1);
  }
  findex_2 = findex;

  d = opendir(folder);
  if (d){
    while ((dir = readdir(d)) != NULL){
      if (dir->d_name[0] != '.'){
        char fullpath[strlen(folder) + strlen(dir->d_name) +8];
        sprintf(fullpath, "%s/%s", folder, dir->d_name);

        if (file_io_string_is_file(fullpath)){
          if (filter == NULL || strcmp(filter, "") == 0 || strstr(dir->d_name, filter) != NULL){
            file_vector[findex] = calloc(sizeof(char) * (strlen(dir->d_name) + 8), 1);
            strcpy(file_vector[findex], dir->d_name);
            findex++;
          }
        }
      }
    }
    closedir(d);
  }

  if (findex - findex_2 > 1){
    quicksort_stringlist(file_vector, findex_2, findex-1);
  }

  return file_vector;
}

int file_io_folder_get_file_n(const char *folder, const char *filter){
  int nfiles = 0;

  DIR *d;
  struct dirent *dir;
  d = opendir(folder);

  if (d){
    while ((dir = readdir(d)) != NULL){
      if (dir->d_name[0] != '.'){
        if (filter == NULL || strcmp(filter, "") == 0 || strstr(dir->d_name, filter) != NULL){
          nfiles ++;
        }
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
