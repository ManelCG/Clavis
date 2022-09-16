#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <file_io.h>

#include <folderstate.h>

typedef struct folderstate{
  char *path;
  int nfiles;
  char **files;
  int state;
  const char *filter;
} folderstate;


void folderstate_empty_list(folderstate *fs){
  for (int i = 0; i < fs->nfiles; i++){
    free(fs->files[i]);
  }
  free(fs->files);
}


folderstate *folderstate_new(const char *folder){
  folderstate *fs = calloc(sizeof(folderstate), 1);
  fs->filter = NULL;
  folderstate_set_filter(fs, "");
  fs->path = (char *) folder;
  fs->nfiles = file_io_folder_get_file_n(folder, fs->filter);
  fs->files = file_io_folder_get_file_list(folder, fs->nfiles, fs->filter);
  fs->state = 0;

  return fs;
}

void folderstate_set_filter(folderstate *fs, const char *filter){
  if (fs->filter != NULL){
    free((char *) fs->filter);
  }
  char *newfilter = calloc(sizeof(char) * (strlen(filter) +8), 1);
  strcpy(newfilter, filter);
  newfilter[strlen(newfilter)] = '\0';
  fs->filter =  newfilter;
}
const char *folderstate_get_filter(folderstate *fs){
  return fs->filter;
}

void folderstate_reload(folderstate *fs){
  folderstate_empty_list(fs);

  fs->nfiles = file_io_folder_get_file_n(fs->path, fs->filter);
  fs->files = file_io_folder_get_file_list(fs->path, fs->nfiles, fs->filter);
  fs->state = 0;
}


int folderstate_chdir(folderstate *fs, const char *folder){
  if (strcmp(folder, "..") == 0){
    int nslashes = 0;
    int last_slash_token = 0;
    for (int i = 0; i < strlen(fs->path); i++){
      if (fs->path[i] == '\0'){
        break;
      }
      if (fs->path[i] == '/'){
        nslashes++;
        last_slash_token = i;
      }
    }

    if (nslashes == 0){
      return -1;
    }

    fs->path[last_slash_token] = '\0';
  } else {
    _Bool correct = false;
    for (int i = 0; i < fs->nfiles; i++){
      if (strcmp(fs->files[i], folder) == 0){
        correct = true;
      }
    }
    if (! correct){
      fprintf(stderr, "Wrong path: %s\n", folder);
      return -1;
    }

    const char *newdir = folderstate_file_get_full_path_from_string(fs, folder);
    fs->path = (char *) newdir;
  }

  folderstate_set_filter(fs, "");
  folderstate_reload(fs);
  return 0;
}

int folderstate_get_nfiles(folderstate *fs){
  return fs->nfiles;
}

const char **folderstate_get_files(folderstate *fs){
  return (const char **) fs->files;
}

int folderstate_get_state(folderstate *fs){
  return fs->state;
}
void folderstate_increase_state(folderstate *fs){
  fs->state++;
  if (fs->state == fs->nfiles){
    fs->state = 0;
  }
}
void folderstate_decrease_state(folderstate *fs){
  fs->state--;
  if (fs->state == -1){
    fs->state = fs->nfiles -1;
  }
}

const char *folderstate_get_path(folderstate *fs){
  return fs->path;
}

const char *folderstate_file_get_full_path_from_string(folderstate *fs, const char *s){
  char *fullpath = calloc(sizeof(char) * (strlen(s) + strlen(fs->path) + 8), 1);
  strcpy(fullpath, fs->path);
  strcat(fullpath, "/");
  strcat(fullpath, s);

  return fullpath;
}

const char *folderstate_file_get_full_path_from_index(folderstate *fs, int index){
  char *s = fs->files[index];

  char *fullpath = calloc(sizeof(char) * (strlen(s) + strlen(fs->path) + 8), 1);
  strcpy(fullpath, fs->path);
  strcat(fullpath, "/");
  strcat(fullpath, s);

  return fullpath;

}

const char *folderstate_get_path_shortname(folderstate *fs){
  char *path;
  if (strcmp(fs->path, ".") == 0){
    path = calloc(sizeof(char) * 32, 1);
    strcpy(path, "Password Store");
  } else {
    path = calloc(sizeof(char) * (strlen(fs->path)+8), 1);
    int nslashes = 0;
    int last_slash_token = 0;
    for (int i = 0; i < strlen(fs->path); i++){
      if (fs->path[i] == '\0'){
        break;
      }
      if (fs->path[i] == '/'){
        nslashes++;
        last_slash_token = i+1;
      }
    }
    strcpy(path, &fs->path[last_slash_token]);
  }
  return path;
}
