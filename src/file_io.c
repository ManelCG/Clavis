#include <sys/stat.h>
#include <dirent.h>

#include <stdbool.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#ifdef __unix__
#include <sys/wait.h>
#elif defined(_WIN32) || defined (WIN32)
#endif

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

    sprintf(path, "%s\\%s\\", getenv("HOMEPATH"), pa);
  #endif
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
    int p_sync[2];
    if (pipe(p) != 0){
      perror("Could not pipe");
      return -1;
    }
    if (pipe(p_sync) != 0){
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

      close(p_sync[0]);

      execlp("pass", "pass", "add", path, NULL);
      exit(-1);
    }

    close(p[0]);
    write(p[1], password, strlen(password));
    write(p[1], "\n", 1);
    write(p[1], password, strlen(password));
    write(p[1], "\n", 1);
    close(p[1]);
    close(p_sync[1]);

    wait(NULL);
    char c;
    while(read(p_sync[0], &c, 1)){
    }
    close(p_sync[0]);

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
  if (stat(s, &pstat) != 0){
    return false;
  }
  return S_ISDIR(pstat.st_mode);
}

_Bool file_io_string_is_file(const char *s){
  struct stat pstat;
  if (stat(s, &pstat) != 0){
    return false;
  }
  return S_ISREG(pstat.st_mode);
}

#ifdef __unix__
char **file_io_get_full_gpg_keys(int *num){
  int npubs, nprivs;
  char **pubs = file_io_get_gpg_keys(&npubs, false);
  char **privs = file_io_get_gpg_keys(&nprivs, true);

  int nkeys = 0;
  char **keys = NULL;

  for (int i = 0; i < npubs; i++){
    for (int j = 0; j < nprivs; j++){
      if (strcmp(pubs[i], privs[j]) == 0){
        nkeys++;
        if (keys == NULL){
          keys = malloc(sizeof(char *) * nkeys);
        } else {
          keys = realloc(keys, sizeof(char *) * nkeys);
        }

        keys[nkeys-1] = malloc(sizeof(char) * (strlen(pubs[i])+1));
        strcpy(keys[nkeys-1], pubs[i]);
      }

      if (i == npubs -1){
        free(privs[j]);
      }
    }


    free(pubs[i]);
  }

  free(pubs);
  free(privs);

  if (num != NULL){
    *num = nkeys;
  }
  return keys;
}

char **file_io_get_gpg_keys(int *num, _Bool secret){
  //gpg --list-keys | grep -E "<*>" | awk '{print $NF}'
  //gpg --list-secret-keys | grep -E "<*>" | awk '{print $NF}'
  int pid;
  int pipe_gpg_grep[2];
  if (pipe(pipe_gpg_grep) != 0){
    perror("Couldnt pipe");
    exit(-1);
  }
  if ((pid = fork()) < 0){
    perror("Couldnt fork");
    exit(pid);
  }

  if (pid == 0){  //Child gpg
    close(1);
    dup(pipe_gpg_grep[1]);
    close(pipe_gpg_grep[1]);
    close(pipe_gpg_grep[0]);


    if (secret){
      execlp("gpg", "gpg", "--list-secret-keys", NULL);
    } else {
      execlp("gpg", "gpg", "--list-keys", NULL);
    }
  }
  //MAIN

  int pipe_grep_awk[2];
  if (pipe(pipe_grep_awk) < 0){
    perror("Couldnt pipe");
    exit(-1);
  }
  if ((pid = fork()) < 0){
    perror("Couldnt fork");
    exit(pid);
  }

  if (pid == 0){  //Child grep
    close(0);
    dup(pipe_gpg_grep[0]);
    close(pipe_gpg_grep[0]);
    close(pipe_gpg_grep[1]);

    close(1);
    dup(pipe_grep_awk[1]);
    close(pipe_grep_awk[1]);
    close(pipe_grep_awk[0]);

    execlp("grep", "grep", "-E", "<*>", NULL);
  }
  //MAIN
  close(pipe_gpg_grep[0]);
  close(pipe_gpg_grep[1]);

  int pipe_awk_main[2];
  if (pipe(pipe_awk_main) < 0){
    perror("Couldnt pipe");
    exit(-1);
  }

  if ((pid = fork()) < 0){
    perror("Couldnt fork");
    exit(pid);
  }

  if (pid == 0){  //Child awk
    close(0);
    dup(pipe_grep_awk[0]);
    close(pipe_grep_awk[0]);
    close(pipe_grep_awk[1]);

    close(1);
    dup(pipe_awk_main[1]);
    close(pipe_awk_main[1]);
    close(pipe_awk_main[0]);

    execlp("awk", "awk", "{print $NF}", NULL);
  }
  //MAIN
  close(pipe_grep_awk[0]);
  close(pipe_grep_awk[1]);
  close(pipe_awk_main[1]);

  wait(NULL);

  //Save output
  int pindex = 0;
  int windex = 0;
  int plen = DEFAULT_GPG_NAME_LEN;
  char **keys = malloc(sizeof(char *));
  keys[pindex] = malloc(sizeof(char) * plen);

  char c;
  while (read(pipe_awk_main[0], &c, 1)){
    if (c == '\n'){
      keys[pindex][windex] = '\0';
      pindex++;
      windex = 0;
      plen = DEFAULT_GPG_NAME_LEN;
      keys = realloc(keys, sizeof(char *) * (pindex+1));
      keys[pindex] = malloc(sizeof(char) * plen);
    } else if (c != '<' && c != '>'){
      keys[pindex][windex] = c;
      windex++;
      if (windex > plen-8){
        plen *= 2;
        keys[pindex] = realloc(keys[pindex], sizeof(char) * plen);
      }
    }
  }
  close(pipe_awk_main[0]);

  if (num != NULL){
    *num = pindex;
  }
  return keys;
}

void file_io_init_password_store(const char *key){
  int pid;
  pid = fork();
  if (pid < 0){
    perror("Could not fork");
  }
  if (pid == 0){
    execlp("pass", "pass", "init", key, NULL);
  }
  wait(NULL);

  pid = fork();
  if (pid < 0){
    perror("Could not fork");
  }
  if (pid == 0){
    execlp("pass", "pass", "git", "init", NULL);
  }
  wait(NULL);
}
#endif
