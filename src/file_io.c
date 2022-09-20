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

#include <sys/stat.h>
#include <dirent.h>

#include <stdbool.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include <libintl.h>
#include <locale.h>

#include <clavis_macros.h>

#ifdef __unix__
#include <sys/wait.h>
#elif defined(_WIN32) || defined (WIN32)
#include <windows.h>
#include <folderstate.h>
#endif

#include <file_io.h>
#include <algorithms.h>
#include <clavis_constants.h>
#include <gui_templates.h>

const char *get_password_store_path(){
  int homelen;
  char *path;

  #ifdef __unix__
    char *passtoredir = getenv("PASSWORD_STORE_DIR");
    if (passtoredir == NULL){
      char *pa = ".password-store";

      homelen = strlen(getenv("HOME"));
      path = malloc(sizeof(char) * (homelen + strlen(pa) + 8));

      sprintf(path, "%s/%s/", getenv("HOME"), pa);
    } else {
      path = malloc(sizeof(char) * (strlen(passtoredir)+1));
      strcpy(path, passtoredir);
    }
  #elif defined(_WIN32) || defined (WIN32)
    char *password_store_path = "Clavis_Passwords";

    homelen = strlen(getenv("HOMEPATH"));
    path = malloc(sizeof(char) * (homelen + strlen(password_store_path) + 8));

    sprintf(path, "C:%s\\%s\\", getenv("HOMEPATH"), password_store_path);
  #endif
  return path;
}

const char *file_io_get_git_config_field(const char *field){
  #ifdef __unix__
  int pid_git;
  int pid_grep;
  int pid_cut;

  int pipe_git_grep[2];
  int pipe_grep_cut[2];
  int pipe_cut_main[2];

  char *return_string;

  if (pipe(pipe_git_grep) < 0){
    perror("Could not pipe");
    return NULL;
  }
  pid_git = fork();
  if (pid_git < 0){
    perror("Could not fork");
    close(pipe_git_grep[0]);
    close(pipe_git_grep[1]);
    return NULL;
  }
  if (pid_git == 0){
    close(1);
    dup(pipe_git_grep[1]);
    close(pipe_git_grep[0]);
    close(pipe_git_grep[1]);

    execlp("git", "git", "config", "--list", NULL);
    return NULL;
  }

  if (pipe(pipe_grep_cut) < 0){
    perror("Could not pipe");
    close(pipe_git_grep[0]);
    close(pipe_git_grep[1]);
    return NULL;
  }
  pid_grep = fork();
  if (pid_grep < 0){
    perror("Could not fork");
    close(pipe_git_grep[0]);
    close(pipe_git_grep[1]);
    close(pipe_grep_cut[0]);
    close(pipe_grep_cut[1]);
    return NULL;
  }
  if (pid_grep == 0){
    close(0);
    dup(pipe_git_grep[0]);
    close(pipe_git_grep[0]);
    close(pipe_git_grep[1]);

    close(1);
    dup(pipe_grep_cut[1]);
    close(pipe_grep_cut[0]);
    close(pipe_grep_cut[1]);

    execlp("grep", "grep", field, NULL);
  }

  close(pipe_git_grep[0]);
  close(pipe_git_grep[1]);

  if (pipe(pipe_cut_main) < 0){
    perror("Could not pipe");
    close(pipe_grep_cut[0]);
    close(pipe_grep_cut[1]);
    return NULL;
  }
  pid_cut = fork();
  if (pid_cut < 0){
    perror("Could not fork");
    close(pipe_grep_cut[0]);
    close(pipe_grep_cut[1]);
    close(pipe_cut_main[0]);
    close(pipe_cut_main[1]);
    return NULL;
  }
  if (pid_cut == 0){
    close(0);
    dup(pipe_grep_cut[0]);
    close(pipe_grep_cut[0]);
    close(pipe_grep_cut[1]);

    close(1);
    dup(pipe_cut_main[1]);
    close(pipe_cut_main[0]);
    close(pipe_cut_main[1]);

    execlp("cut", "cut", "-d=", "-f2", NULL);
  }

  close(pipe_grep_cut[0]);
  close(pipe_grep_cut[1]);

  wait(NULL);

  //Read
  size_t stringlen = 32;
  int index = 0;
  return_string = calloc(sizeof(char) * stringlen, 1);
  char c;

  close(pipe_cut_main[1]);
  while (read(pipe_cut_main[0], &c, sizeof(c))){
    if (c != '\0' && c != '\n'){
      if (index == stringlen){
        stringlen *= 2;
        return_string = realloc(return_string, stringlen);
      }

      return_string[index] = c;
      index++;
    } else {
      break;
    }
  }

  if (index == stringlen){
    stringlen += 2;
    return_string = realloc(return_string, stringlen);
  }
  return_string[index] = '\0';

  close(pipe_cut_main[0]);

  return return_string;
  #elif defined(_WIN32) || defined (WIN32)
  return NULL;
  #endif
}

_Bool file_io_rm_rf(const char *path){
  _Bool removed = false;
  if (file_io_string_is_file(path)){
    if (file_io_remove_password(path) == 0){
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
          if (file_io_remove_password(filepath) == 0){
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

int file_io_init_git_server(const char *username, const char *email, const char *repo_url, _Bool create_new, _Bool refactor_git){
  #ifdef __unix__
  int pid;

  if (repo_url != NULL && !create_new){
    int authmethod = file_io_get_git_auth_method_from_string(repo_url);
    if (authmethod == CLAVIS_GIT_AUTH_SSH){
      if ((pid = fork()) < 0){
        perror("Could not fork");
        return -1;
      }
      if (pid == 0){
        execlp("git", "git", "clone", repo_url, ".", NULL);
        return -1;
      }
      waitpid(pid, NULL, 0);
    } else if (authmethod != CLAVIS_GIT_NONE) {
      char *credentials = gui_templates_ask_for_git_credentials();
      if (credentials != NULL){
        int p[2];
        if (pipe(p) < 0){
          perror("Could not pipe");
          return CLAVIS_ERROR_PIPE;
        }
        if ((pid = fork()) < 0){
          perror("Could not fork");
          return CLAVIS_ERROR_FORK;
        }
        if (pid == 0){
          close(0);
          dup(p[0]);
          close(p[1]);
          close(p[0]);

          execlp("git", "git", "clone", repo_url, ".", NULL);
          return CLAVIS_ERROR_EXECLP;
        }
        close(p[0]);

        wait(NULL);
        printf(_("Writing credentials:\n%s\n"), credentials);
        write(p[1], credentials, strlen(credentials));
        printf("_(Done\n");
        close(p[1]);
        free(credentials);
      }
    }
  }

  if (create_new){
    if ((pid = fork()) < 0){
      perror("Could not fork");
      return -1;
    }
    if (pid == 0){
      execlp("pass", "pass", "git", "init", NULL);
      return -1;
    }
    waitpid(pid, NULL, 0);
  }

  if (email != NULL){
    if ((pid = fork()) < 0){
      perror("Could not fork");
      return -1;
    }
    if (pid == 0){
      execlp("git", "git", "config", "user.email", email, NULL);
      return -1;
    }
    waitpid(pid, NULL, 0);
  }

  if (username != NULL){
    if ((pid = fork()) < 0){
      perror("Could not fork");
      return -1;
    }
    if (pid == 0){
      execlp("git", "git", "config", "user.name", username, NULL);
      return -1;
    }
    waitpid(pid, NULL, 0);
  }

  if (create_new){
    if (repo_url != NULL){
      if ((pid = fork()) < 0){
        perror("Could not fork");
        return -1;
      }
      if (pid == 0){
        if(!refactor_git){
          execlp("git", "git", "remote", "add", "origin", repo_url, NULL);
        } else {
          execlp("git", "git", "remote", "set-url", "origin", repo_url, NULL);
        }
        return -1;
      }
      waitpid(pid, NULL, 0);

      if ((pid = fork()) < 0){
        perror("Could not fork");
        return -1;
      }
      if (pid == 0){
        execlp("git", "git", "config", "pull.rebase", "false", NULL);
        return -1;
      }
      waitpid(pid, NULL, 0);

      if ((pid = fork()) < 0){
        perror("Could not fork");
        return -1;
      }
      if (pid == 0){
        execlp("git", "git", "push", "--set-upstream", "origin", "master", NULL);
        return -1;
      }
      waitpid(pid, NULL, 0);
    }
  }
  return 0;
  #elif defined(_WIN32) || defined (WIN32)
  if (repo_url != NULL){
    if (!create_new){
      char git_params[strlen(repo_url) + 64];
      sprintf(git_params, "git.exe clone \"%s\" .", repo_url);

      perform_git_command(git_params);
    }
  }

  if (create_new){
    perform_git_command("git.exe init .");
  }

  if (email != NULL){
    char git_params[strlen(email) + 64];
    sprintf(git_params, "git.exe config user.email \"%s\"", email);

    perform_git_command(git_params);
  }

  if (username != NULL){
    char git_params[strlen(username) + 64];
    sprintf(git_params, "git.exe config user.name \"%s\"", username);

    perform_git_command(git_params);
  }



  if (create_new){
    perform_git_command("git.exe add *");
    perform_git_command("git.exe add .gpg-id");
    perform_git_command("git.exe commit -m \"Added current contents of password store\"");

    if (!refactor_git){
      char git_params[strlen(repo_url) + 128];
      sprintf(git_params, "git.exe remote add origin \"%s\"", repo_url);
      perform_git_command(git_params);
    } else {
      char git_params[strlen(repo_url) + 128];
      sprintf(git_params, "git.exe remote set-url origin \"%s\"", repo_url);
      perform_git_command(git_params);
    }

    perform_git_command("git.exe config pull.rebase false");
    perform_git_command("git.exe push --set-upstream origin master");
  }
  return 0;
  #endif
}

int file_io_get_file_count(const char *path, _Bool recursive){
  if (file_io_string_is_folder(path)){
    int nfiles = file_io_folder_get_file_n(path, "");
    char **files = file_io_folder_get_file_list(path, nfiles, "");

    int count = 0;

    for (int i = 0; i < nfiles; i++){
      char filepath[strlen(path) + strlen(files[i]) + 8];
      sprintf(filepath, "%s/%s", path, files[i]);

      if (file_io_string_is_file(filepath)){
        if (strlen(filepath) > 4 && strcmp(&filepath[strlen(filepath)-4], ".gpg") == 0){
          count += 1;
        }
      } else if (recursive){
        int n = file_io_get_file_count(filepath, true);
        if (n > 0){
          count += n;
        }
      }

      free(files[i]);
    }

    free(files);
    return count;
  } else if (file_io_string_is_file(path)){
    return 0;
  } else {
    return -1;
  }
}

int file_io_get_git_auth_method_from_string(const char *url){
  printf("Get auth method from string\n");

  char *substring;
  char buffer_git[strlen(url)+2];
  char comp_buffer[6];
  strcpy(buffer_git, url);
  buffer_git[strlen(url)+1] = '\0';

  if (strlen(buffer_git) == 0){
    return CLAVIS_GIT_NONE;
  }

  strncpy(comp_buffer, buffer_git, 4);
  comp_buffer[4] = '\0';

  printf("%s\n", comp_buffer);

  if (strcmp(comp_buffer, "git@") == 0 || strcmp(comp_buffer, "git:") == 0){
    printf("ssh\n");
    return CLAVIS_GIT_AUTH_SSH;
  }


  substring = strstr(buffer_git, "github.com");
  char github_buffer[11];
  if (substring != NULL){
    strncpy(github_buffer, substring, 10);
    github_buffer[10] = '\0';

    printf("here\n%s\n", github_buffer);

    if (strcmp(github_buffer, "github.com") == 0){
      printf("Github\n");
      return CLAVIS_GIT_AUTH_HTTPS_GITHUB;
    }
  }

  if (strcmp(comp_buffer, "http") == 0){
    printf("Https\n");
    return CLAVIS_GIT_AUTH_HTTPS;
  }

  return CLAVIS_GIT_NONE;
}

int file_io_get_git_auth_method(){
  printf("Get auth method\n");
  char buffer_git[4096];
  buffer_git[0] = '\0';

  #ifdef __unix__
  int p[2];
  int pid;
  if (pipe(p) < 0){
    perror("Could not pipe");
    return CLAVIS_ERROR_PIPE;
  }

  pid = fork();
  if (pid < 0){
    perror("Could not fork");
    return CLAVIS_ERROR_FORK;
  }

  if (pid == 0){
    close(1);
    dup(p[1]);
    close(p[1]);
    close(p[0]);

    execlp("git", "git", "remote", "show", "origin", NULL);
    return CLAVIS_ERROR_EXECLP;
  }

  close(p[1]);
  size_t bytes = read(p[0], buffer_git, 4096);
  buffer_git[bytes] = '\0';
  #elif defined(_WIN32) || defined (WIN32)
  HANDLE child_OUT_rd = NULL;
  HANDLE child_OUT_wr = NULL;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = true;
  saAttr.lpSecurityDescriptor = NULL;

  CreatePipe(&child_OUT_rd, &child_OUT_wr, &saAttr, 0);
  SetHandleInformation(child_OUT_rd, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = child_OUT_wr;
  siStartInfo.hStdInput = NULL;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  CreateProcessA("C:\\Program Files\\Git\\cmd\\git.exe",
                 "git.exe remote show origin",
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
  CloseHandle(child_OUT_wr);

  int index = 0;
  while(ReadFile(child_OUT_rd, &buffer_git[index], 1, NULL, NULL)){
    index++;
  }
  CloseHandle(child_OUT_rd);
  #endif

  if (strlen(buffer_git) == 0){
    printf("None\n");
    return CLAVIS_GIT_NONE;
  }

  char *token;
  char *substring = strstr(buffer_git, "Fetch URL:");
  char comp_buffer[5];

  if (strlen(buffer_git) == 0){
    printf("None2\n");
    return CLAVIS_GIT_NONE;
  }

  substring = strstr(buffer_git, ":");
  substring += 2;

  token = strstr(substring, "\n");
  token[0] = '\0';

  printf("%s\n", substring);

  strncpy(comp_buffer, substring, 4);

  printf("%s\n", comp_buffer);
  if (strcmp(comp_buffer, "git@") == 0 || strcmp(comp_buffer, "git:") == 0){
    return CLAVIS_GIT_AUTH_SSH;
  }

  substring = strstr(buffer_git, "github.com");
  char github_buffer[11];
  strncpy(github_buffer, substring, 10);

  if (strcmp(github_buffer, "github.com") == 0){
    printf("Github!!\n");
    return CLAVIS_GIT_AUTH_HTTPS_GITHUB;
  }

  if (strcmp(comp_buffer, "http") == 0){
    return CLAVIS_GIT_AUTH_HTTPS;
  }

  return CLAVIS_GIT_NONE;
}

int file_io_remove_password(const char *path){
  #ifdef __unix__
  char filepath[strlen(path)+1];
  strcpy(filepath, path);
  if (strlen(filepath) > 4 && strcmp(&filepath[strlen(filepath)-4], ".gpg") == 0){
    filepath[strlen(filepath)-4] = '\0';
  }

  int pid;
  int p_sync[2];
  int p[2];
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

  if (pid == 0){
    close(0);
    dup(p[0]);
    close(p[0]);
    close(p[1]);

    execlp("pass", "pass", "rm", filepath, NULL);
    return -1;
  }

  close(p[0]);
  write(p[1], "y\n", 2);
  close(p[1]);

  wait(NULL);
  close(p_sync[1]);
  char c;
  while (read(p_sync[0], &c, 1)){}
  close(p_sync[0]);

  return 0;
  #elif defined(_WIN32) || defined (WIN32)
  int ret;
  ret =remove(path);
  if (ret != 0){
    return ret;
  }

  char args_git[strlen(path) + 64];

  sprintf(args_git, "git.exe rm %s", path);
  perform_git_command(args_git);

  sprintf(args_git, "git.exe commit -m \"Removed password %s\"", path);
  perform_git_command(args_git);

  return 0;
  #endif
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
    char *gpgid = file_io_get_gpgid();
    if (gpgid == NULL){
      return -1;
    }

    char *token = strchr(gpgid, '\n');
    if (token != NULL){
      token[0] = '\0';
    }
    token = strchr(gpgid, '\r');
    if (token != NULL){
      token[0] = '\0';
    }

    HANDLE child_IN_rd = NULL;
    HANDLE child_IN_wr = NULL;

    HANDLE child_OUT_rd = NULL;
    HANDLE child_OUT_wr = NULL;

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = true;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&child_IN_rd, &child_IN_wr, &saAttr, 0);
    SetHandleInformation(child_IN_wr, HANDLE_FLAG_INHERIT, 0);

    CreatePipe(&child_OUT_rd, &child_OUT_wr, &saAttr, 0);
    SetHandleInformation(child_OUT_rd, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = NULL;
    siStartInfo.hStdOutput = child_OUT_wr;
    siStartInfo.hStdInput = child_IN_rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    char gpg_parms[strlen(gpgid) + 64];
    sprintf(gpg_parms, "gpg.exe --encrypt --armor -r \"%s\"", gpgid);
    printf("%s\n", gpg_parms);

    CreateProcess("C:\\Program Files (x86)\\GnuPG\\bin\\gpg.exe",
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
    CloseHandle(child_OUT_wr);
    CloseHandle(child_IN_rd);

    WriteFile(child_IN_wr, password, strlen(password), NULL, NULL);
    CloseHandle(child_IN_wr);

    HANDLE hFile;
    hFile = CreateFile(path,
                       GENERIC_WRITE,
                       0,
                       NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);

    if (hFile == INVALID_HANDLE_VALUE){
      return -1;
    }

    char c;
    printf("Stderr:\n");
    while (ReadFile(child_OUT_rd, &c, 1, NULL, NULL)){
      WriteFile(hFile, &c, 1, NULL, NULL);
    }
    CloseHandle(child_OUT_rd);
    CloseHandle(hFile);

    free(gpgid);

    return 0;
  #endif
}

char *file_io_get_gpgid(){
  #ifdef __unix__
  char *key_id = malloc(sizeof(char) * 512);
  int fd = open(".gpg-id", O_RDONLY);

  if (fd < 0){
    return NULL;
  }

  int len = read(fd, key_id, 512);
  if (len == 0){
    return NULL;
  }

  key_id[len-1] = '\0';

  return key_id;
  #elif defined(_WIN32) || defined (WIN32)
  HANDLE hFile;
  hFile = CreateFile(".gpg-id",
                     GENERIC_READ,
                     FILE_SHARE_READ,
                     NULL,
                     OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);

  if (hFile == INVALID_HANDLE_VALUE){
    return NULL;
  }

  DWORD bread;
  char *ret = malloc(sizeof(char) * 4096);

  ReadFile(hFile, ret, 4095, &bread, NULL);
  ret[bread] = '\0';

  CloseHandle(hFile);
  return ret;
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
    char *gpgid = file_io_get_gpgid();
    if (gpgid == NULL){
      return NULL;
    }

    HANDLE child_OUT_rd = NULL;
    HANDLE child_OUT_wr = NULL;

    SECURITY_ATTRIBUTES saAttr;
    saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
    saAttr.bInheritHandle = true;
    saAttr.lpSecurityDescriptor = NULL;

    CreatePipe(&child_OUT_rd, &child_OUT_wr, &saAttr, 0);
    SetHandleInformation(child_OUT_rd, HANDLE_FLAG_INHERIT, 0);

    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = NULL;
    siStartInfo.hStdOutput = child_OUT_wr;
    siStartInfo.hStdInput = NULL;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    char gpg_parms[strlen(file) + 64];
    sprintf(gpg_parms, "gpg.exe --decrypt \"%s\"", file);

    printf("DECRYPTING %s\n", file);
    printf("%s\n", gpg_parms);

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
    CloseHandle(child_OUT_wr);

    size_t passlen = DEFAULT_PASSWORD_SIZE;
    int index = 0;
    char *decrypted_password = malloc(sizeof(char) * passlen);
    char c;

    while (ReadFile(child_OUT_rd, &c, 1, NULL, NULL)){
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
    CloseHandle(child_OUT_rd);

    if (index == passlen){
      passlen += 2;
      decrypted_password = realloc(decrypted_password, passlen);
    }
    decrypted_password[index] = '\0';

    free(gpgid);
    return decrypted_password;
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

int cp(const char *from, const char *to){
  int fd_to, fd_from;
  char buf[4096];
  ssize_t nread;
  int saved_errno;

  fd_from = open(from, O_RDONLY);
  if (fd_from < 0){
    return -1;
  }

  fd_to = open(to, O_WRONLY | O_CREAT | O_TRUNC, 0666);
  if (fd_to < 0){
    goto out_error;
  }

  while (nread = read(fd_from, buf, sizeof buf), nread > 0){
    char *out_ptr = buf;
    ssize_t nwritten;

    do {
      nwritten = write(fd_to, out_ptr, nread);

      if (nwritten >= 0){
        nread -= nwritten;
        out_ptr += nwritten;
      } else if (errno != EINTR){
        goto out_error;
      }
    } while (nread > 0);
  }

  if (nread == 0){
    if (close(fd_to) < 0){
      fd_to = -1;
      goto out_error;
    }
    close(fd_from);

    //Success!
    return 0;
  }

  out_error:
    saved_errno = errno;
    close(fd_from);
    if (fd_to >= 0){
      close(fd_to);
    }

    errno = saved_errno;
    return -1;
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
  #ifdef __unix__
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
      execlp("gpg", "gpg", "--list-secret-keys", "--with-colons", NULL);
    } else {
      execlp("gpg", "gpg", "--list-keys", "--with-colons", NULL);
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

    execlp("grep", "grep", "-E", "<*>|^uid", NULL);
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

    execlp("awk", "awk", "-F:", "{print $10}", NULL);
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
    } else /*if (c != '<' && c != '>')*/{
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
  #elif defined(_WIN32) || defined (WIN32)
  //gpg --list-keys --with-colons
  //gpg --list-secret-keys --with-colons
  HANDLE gpg_OUT_rd = NULL;
  HANDLE gpg_OUT_wr = NULL;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = true;
  saAttr.lpSecurityDescriptor = NULL;

  CreatePipe(&gpg_OUT_rd, &gpg_OUT_wr, &saAttr, 0);
  SetHandleInformation(gpg_OUT_rd, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = gpg_OUT_wr;
  siStartInfo.hStdInput = NULL;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  if (secret){
    CreateProcessA("C:\\Program Files (x86)\\GnuPG\\bin\\gpg.exe",
                   "gpg.exe --list-secret-keys --with-colons",
                   NULL,
                   NULL,
                   true,
                   CREATE_NO_WINDOW,
                   NULL,
                   NULL,
                   &siStartInfo,
                   &piProcInfo);
  } else {
    CreateProcessA("C:\\Program Files (x86)\\GnuPG\\bin\\gpg.exe",
                   "gpg.exe --list-keys --with-colons",
                   NULL,
                   NULL,
                   true,
                   CREATE_NO_WINDOW,
                   NULL,
                   NULL,
                   &siStartInfo,
                   &piProcInfo);
  }
  CloseHandle(piProcInfo.hProcess);
  CloseHandle(piProcInfo.hThread);
  CloseHandle(gpg_OUT_wr);

  int buffer_size = 128;
  char *buffer = malloc(sizeof(char) * buffer_size);
  buffer[0] = '\0';
  char c;
  int i = 0;

  while (ReadFile(gpg_OUT_rd, &c, 1, NULL, NULL)){
    buffer[i] = c;

    i++;
    if (i == buffer_size){
      buffer_size *= 2;
    }
    buffer = realloc(buffer, sizeof(char) * buffer_size);
    buffer[i] = '\0';
  }

  CloseHandle(gpg_OUT_rd);

  //Vars for saving output
  int pindex = 0;
  char **keys = NULL;

  //grep "^uid:"
  unsigned int line_buff_size = 4096;
  char line_buffer[line_buff_size];
  char aux_buffer[8];
  char *token = buffer;
  char *last_token;
  long long int len;
  i = 0;

  while (1 && buffer[0] != '\0'){
    last_token = token;
    token = strchr(token, '\n');
    len = token - last_token;

    if (len < line_buff_size){
      snprintf(line_buffer, len, last_token);
      snprintf(aux_buffer, 5, line_buffer);
      if (strcmp(aux_buffer, "uid:") == 0){
        //awk -F: '{print $10}'
        char *colon_token = line_buffer;
        char *last_colon_token;
        for (int i = 0; i < 9; i++){
          colon_token = strchr(colon_token, ':');
          colon_token++;
        }
        last_colon_token = strchr(colon_token, ':');
        last_colon_token[0] = '\0';

        //Save output
        if (keys == NULL){
          keys = malloc(sizeof(char *));
        } else {
          keys = realloc(keys, sizeof(char *) * (pindex +1));
        }
        keys[pindex] = malloc(sizeof(char) * ((last_colon_token-colon_token)+2));
        strcpy(keys[pindex], colon_token);
        pindex++;
      }
    }

    if (token[0] == '\n'){
      token++;
    }
    if (token[0] == '\0'){
      break;
    }

    i++;
  }

  free(buffer);


  if (num != NULL){
    *num = pindex;
  }
  return keys;
  #endif
}

void file_io_init_password_store(const char *key){
#ifdef __unix__
  int pid;
  pid = fork();
  if (pid < 0){
    perror("Could not fork");
    exit(-1);
  }
  if (pid == 0){
    execlp("pass", "pass", "init", key, NULL);
  }
  wait(NULL);

#elif defined(_WIN32) || defined (WIN32)
  void re_encrypt_file(const char *path){
    printf("Re encrypting %s\n", path);
    const char *pw = file_io_decrypt_password(path);
    if (pw != NULL){
      file_io_encrypt_password(pw, path);
      free((char *) pw);
    }
  }

  void re_encrypt_folder(const char *path){
    DIR *d;
    struct dirent *dir;
    d = opendir(path);

    if (d){
      while ((dir = readdir(d)) != NULL){
        char fullpath[strlen(dir->d_name) + strlen(path) + 2];
        sprintf(fullpath, "%s\\%s", path, dir->d_name);
        if (file_io_string_is_file(fullpath)){
          if (strcmp(dir->d_name, ".gpg-id") != 0){
            re_encrypt_file(fullpath);
          }
        } else if (file_io_string_is_folder(fullpath)){
          if (dir->d_name[0] != '.'){
            if (!file_io_folder_has_gpgid(fullpath)){
              re_encrypt_folder(fullpath);
            }
          }
        }
      }

      closedir(d);
    }
  }

  HANDLE hFile;
  hFile = CreateFile(".gpg-id",
                     GENERIC_WRITE,
                     0,
                     NULL,
                     CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);

  if (hFile == INVALID_HANDLE_VALUE){
    return;
  }

  WriteFile(hFile, key, strlen(key), NULL, NULL);
  WriteFile(hFile, "\n", 1, NULL, NULL);

  CloseHandle(hFile);

  const char *cwd = _getcwd(NULL, 0);
  re_encrypt_folder(cwd);
#endif
}

void file_io_gpg_trust_key(const char *key){
#ifdef __unix__
  int pid;
  int p[2];
  if (pipe(p) < 0){
    perror("Could not pipe");
    exit(-1);
  }
  pid = fork();
  if (pid < 0){
    perror("Could not fork");
    exit(-1);
  }

  if (pid == 0){
    close(0);
    dup(p[0]);
    close(p[1]);
    close(p[0]);

    execlp("gpg", "gpg", "--command-fd", "0", "--expert", "--edit-key", key, "trust", NULL);
  }

  close(p[0]);
  write(p[1], "5\ny\n", 4);
  close(p[1]);
  waitpid(pid, NULL, 0);
#elif defined(_WIN32) || defined (WIN32)
  HANDLE child_SYNC_rd = NULL;
  HANDLE child_SYNC_wr = NULL;

  HANDLE child_IN_rd = NULL;
  HANDLE child_IN_wr = NULL;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = true;
  saAttr.lpSecurityDescriptor = NULL;

  CreatePipe(&child_SYNC_rd, &child_SYNC_wr, &saAttr, 0);
  SetHandleInformation(child_SYNC_rd, HANDLE_FLAG_INHERIT, 0);

  CreatePipe(&child_IN_rd, &child_IN_wr, &saAttr, 0);
  SetHandleInformation(child_IN_wr, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = child_SYNC_wr;
  siStartInfo.hStdInput = child_IN_rd;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  char gpg_parms[strlen(key) + 128];
  sprintf(gpg_parms, "gpg.exe --command-fd 0 --expert --edit-key \"%s\" trust", key);
  printf("%s\n", gpg_parms);

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
  CloseHandle(child_IN_rd);

  WriteFile(child_IN_wr, "5\ny\n", 4, NULL, NULL);
  CloseHandle(child_IN_wr);

  char blackhole;
  while(ReadFile(child_SYNC_rd, &blackhole, 1, NULL, NULL)){
  }
  CloseHandle(child_SYNC_rd);
#endif
}

void file_io_export_gpg_keys(const char *key, const char *path, _Bool private){
#ifdef __unix__
  int pid = fork();
  if (pid < 0){
    perror("Could not fork");
    return;
  }
  if (pid == 0){  //Child
    close(1);
    fopen(path, "w");

    if (private){
      execlp("gpg", "gpg", "--export-secret-key", "-a", key, NULL);
      exit(-1);
    } else {
      execlp("gpg", "gpg", "--export", "-a", key, NULL);
      exit(-1);
    }
  }
  return;
#elif defined(_WIN32) || defined (WIN32)
  HANDLE child_OUT_rd = NULL;
  HANDLE child_OUT_wr = NULL;

  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = true;
  saAttr.lpSecurityDescriptor = NULL;

  CreatePipe(&child_OUT_rd, &child_OUT_wr, &saAttr, 0);
  SetHandleInformation(child_OUT_rd, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = NULL;
  siStartInfo.hStdOutput = child_OUT_wr;
  siStartInfo.hStdInput = NULL;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  char gpg_parms[strlen(key) + 64];
  if (private){
    sprintf(gpg_parms, "gpg.exe --export-secret-key -a '%s'", key);
  } else {
    sprintf(gpg_parms, "gpg.exe --export -a '%s'", key);
  }

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
  CloseHandle(child_OUT_wr);

  HANDLE hFile;
  hFile = CreateFile(path,
                     GENERIC_WRITE,
                     0,
                     NULL,
                     CREATE_ALWAYS,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);

  if (hFile == INVALID_HANDLE_VALUE){
    return;
  }


  char c;
  while (ReadFile(child_OUT_rd, &c, 1, NULL, NULL)){
    WriteFile(hFile, &c, 1, NULL, NULL);
  }

  CloseHandle(hFile);
  return;
#endif
}

_Bool file_io_folder_has_gpgid(const char *path){
  DIR *d;
  struct dirent *dir;
  d = opendir(path);
  _Bool has_gpgid = false;

  if (d){
    while ((dir = readdir(d)) != NULL){
      if (strcmp(dir->d_name, ".gpg-id") == 0){
        has_gpgid = true;
      }
    }

    closedir(d);
  }

  return has_gpgid;
}

void perform_git_command(char *args){
  #if defined(_WIN32) || defined (WIN32)
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = true;
  saAttr.lpSecurityDescriptor = NULL;

  HANDLE child_SYNC_rd = NULL;
  HANDLE child_SYNC_wr = NULL;

  HANDLE child_ERR_rd = NULL;
  HANDLE child_ERR_wr = NULL;

  CreatePipe(&child_SYNC_rd, &child_SYNC_wr, &saAttr, 0);
  SetHandleInformation(child_SYNC_rd, HANDLE_FLAG_INHERIT, 0);

  CreatePipe(&child_ERR_rd, &child_ERR_wr, &saAttr, 0);
  SetHandleInformation(child_ERR_rd, HANDLE_FLAG_INHERIT, 0);

  PROCESS_INFORMATION piProcInfo;
  STARTUPINFO siStartInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
  siStartInfo.cb = sizeof(STARTUPINFO);
  siStartInfo.hStdError = child_ERR_wr;
  siStartInfo.hStdOutput = child_SYNC_wr;
  siStartInfo.hStdInput = NULL;
  siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

  printf("Performing %s\n", args);
  CreateProcessA("C:\\Program Files\\Git\\cmd\\git.exe",
                 args,
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
  CloseHandle(child_ERR_wr);

  WaitForSingleObject(piProcInfo.hProcess, INFINITE);

  char blackhole;
  printf("STDOUT:\n");
  while(ReadFile(child_SYNC_rd, &blackhole, 1, NULL, NULL)){
    printf("%c", blackhole);
  }
  printf("STDERR:\n");
  while(ReadFile(child_ERR_rd, &blackhole, 1, NULL, NULL)){
    printf("%c", blackhole);
  }

  printf("Finished syncing %s\n", args);
  CloseHandle(child_SYNC_rd);
  CloseHandle(child_ERR_rd);
  #endif
}

#ifdef __unix__
#ifndef MAKE_INSTALL
char *file_io_get_about_picture_png(){
  char *assetspath = file_io_get_clavis_folder();
  char *about_picture_subpath = "assets/inapp_assets/about_picture.png";

  char *ret = malloc(strlen(assetspath) + strlen(about_picture_subpath) + 8);
  sprintf(ret, "%s/%s", assetspath, about_picture_subpath);

  free(assetspath);
  return ret;
}
#endif
#endif
#if defined(_WIN32) || defined (WIN32)
char *file_io_get_about_picture_png(){
  char *assetspath = file_io_get_clavis_folder();
  char *about_picture_subpath = "assets\\inapp_assets\\about_picture.png";

  char *ret = malloc(strlen(assetspath) + strlen(about_picture_subpath) + 8);
  sprintf(ret, "%s\\%s", assetspath, about_picture_subpath);

  free(assetspath);
  return ret;
}

char *file_io_get_gtk_settings_ini_file(){
  char *assetspath = file_io_get_clavis_folder();
  char *about_picture_subpath = "etc\\gtk-3.0\\settings.ini";

  char *ret = malloc(strlen(assetspath) + strlen(about_picture_subpath) + 8);
  sprintf(ret, "%s\\%s", assetspath, about_picture_subpath);

  free(assetspath);
  return ret;
}

int file_io_get_gtk_theme(){
  {
    char *appdata = getenv("APPDATA");
    char themefile[strlen(appdata) + strlen("Clavis\\clavis_use_darktheme") + 8];
    sprintf(themefile, "%s\\%s", appdata, "Clavis\\clavis_use_darktheme");

    if (file_io_string_is_file(themefile)){
      HANDLE hFile;
      hFile = CreateFile(themefile,
                         GENERIC_READ,
                         FILE_SHARE_READ,
                         NULL,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         NULL);

      if (hFile != INVALID_HANDLE_VALUE){
        DWORD bread;
        char c;

        ReadFile(hFile, &c, 1, &bread, NULL);
        CloseHandle(hFile);

        if (c == '1'){
          return CLAVIS_THEME_DARK;
        } else if (c == '0'){
          return CLAVIS_THEME_LIGHT;
        }
      }

    }
  }
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
    return CLAVIS_THEME_UNDEFINED;
  }

  DWORD bread;
  char *buffer = malloc(sizeof(char) * 4096);

  ReadFile(hFile, buffer, 4095, &bread, NULL);
  buffer[bread] = '\0';

  CloseHandle(hFile);
  free(f_ini);

  unsigned int line_buff_size = 4096;
  char line_buffer[line_buff_size];
  char aux_buffer[64];
  char *token = buffer;
  char *last_token;
  long long int len;

  int theme = CLAVIS_THEME_UNDEFINED;

  while (1 && buffer[0] != '\0'){
    last_token = token;
    token = strchr(token, '\n');
    len = token - last_token;

    if (len < line_buff_size && len > 34){
      snprintf(line_buffer, len+1, last_token);
      snprintf(aux_buffer, 34, line_buffer);
      if (strcmp(aux_buffer, "gtk-application-prefer-dark-theme") == 0){
        char *value;
        value = strstr(line_buffer, "false");
        if (value != NULL){
          theme = CLAVIS_THEME_LIGHT;
        }
        value = strstr(line_buffer, "true");
        if (value != NULL){
          theme = CLAVIS_THEME_DARK;
        }
      }
    }

    if (token[0] == '\n'){
      token++;
    }
    if (token[0] == '\0'){
      break;
    }
  }

  free(buffer);

  return theme;
}
#endif

char *file_io_get_clavis_folder(){
  char *ret;
  #ifdef MAKE_INSTALL
  ret = malloc(strlen("/usr/lib/clavis/") + 2);
  strcpy(ret, "/usr/lib/clavis/");
  return ret;
  #elif defined __unix__
  ret = malloc(sizeof(char) * 1024);
  int written = readlink("/proc/self/exe", ret, 1023);
  ret[written] = '\0';
  char *token = strrchr(ret, '/');
  if (token[0] == '/'){
    token[0] = '\0';
  }
  token = strrchr(ret, '/');
  if (token[0] == '/'){
    token[1] = '\0';
  }
  return ret;
  #elif defined(_WIN32) || defined (WIN32)
  int size = MAX_PATH;
  ret = malloc(sizeof(char) * MAX_PATH);
  GetModuleFileName(NULL, ret, size - 1);
  char *token = strrchr(ret, '\\');
  if (token[0] == '\\'){
    token[1] = '\0';
  }
  return ret;
  #endif
  return NULL;
}

char *file_io_get_clavis_executable(){
  char *ret;
  #if defined __unix__
  ret = malloc(sizeof(char) * 1024);
  readlink("/proc/self/exe", ret, 1023);
  return ret;
  #elif defined(_WIN32) || defined (WIN32)
  int size = MAX_PATH;
  ret = malloc(sizeof(char) * MAX_PATH);
  GetModuleFileName(NULL, ret, size - 1);
  return ret;
  #endif
}

char *file_io_get_clavis_license_file_buffer(){
  #ifdef __unix__
  char *folder = file_io_get_clavis_folder();
  char *license = malloc((strlen(folder) + strlen("LICENSE") + 8) * sizeof(char));
  sprintf(license, "%s/%s", folder, "LICENSE");

  size_t size;
  FILE *fp = fopen(license, "r");
  fseek(fp, 0L, SEEK_END);
  size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  char *buffer = malloc(sizeof(char) * size);
  fread(buffer, 1, size, fp);

  fclose(fp);
  #elif defined(_WIN32) || defined (WIN32)
  char *folder = file_io_get_clavis_folder();
  char *license = malloc((strlen(folder) + strlen("LICENSE") + 8) * sizeof(char));
  sprintf(license, "%s\\%s", folder, "LICENSE");

  HANDLE hFile;
  hFile = CreateFile(license,
                     GENERIC_READ,
                     FILE_SHARE_READ,
                     NULL,
                     OPEN_EXISTING,
                     FILE_ATTRIBUTE_NORMAL,
                     NULL);

  if (hFile == INVALID_HANDLE_VALUE){
    return NULL;
  }

  size_t size = GetFileSize(hFile, NULL);
  char *buffer = malloc(sizeof(char) * size);

  ReadFile(hFile, buffer, size, NULL, NULL);
  CloseHandle(hFile);
  #endif

  free(license);
  free(folder);

  return buffer;
}

#if defined(_WIN32) || defined (WIN32)
char *windows_string(const char *s){
  size_t size = mbstowcs(NULL, s, 0);
  wchar_t *wide = malloc(sizeof(wchar_t) * (size + 2));
  size = mbstowcs(wide, s, size);
  wide[size] = '\0';
  wide[size+1] = '\0';

  char *ret = g_utf16_to_utf8(wide, -1, NULL, NULL, NULL);
  free(wide);
  return ret;
}
#endif
