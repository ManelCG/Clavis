#include <sys/stat.h>
#include <dirent.h>

#include <stdbool.h>

#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#ifdef __unix__
#include <sys/wait.h>
#elif defined(_WIN32) || defined (WIN32)
#include <windows.h>
#include <folderstate.h>
#endif

#include <file_io.h>
#include <algorithms.h>
#include <clavis_constants.h>

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
    #ifdef __unix__
    if (file_io_remove_password(path) == 0){
    #elif defined(_WIN32) || defined (WIN32)
    if (remove(path) == 0){
    #endif
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
          #ifdef __unix__
          if (file_io_remove_password(filepath) == 0){
          #elif defined(_WIN32) || defined (WIN32)
          if (remove(filepath) == 0){
          #endif
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

  if (repo_url != NULL){
    if (! create_new){
      if ((pid = fork()) < 0){
        perror("Could not fork");
        return -1;
      }
      if (pid == 0){
        execlp("git", "git", "clone", repo_url, ".", NULL);
        return -1;
      }
      waitpid(pid, NULL, 0);
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

#ifdef __unix__
int file_io_remove_password(const char *path){
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
}
#endif

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

    CreateProcessA("C:\\Program Files (x86)\\GnuPG\\bin\\gpg.exe",
                   gpg_parms,
                   NULL,
                   NULL,
                   true,
                   0,
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
      return;
    }

    char c;
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
                   0,
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
                   0,
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

  CloseHandle(hFile);
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
  wait(NULL);
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
                 0,
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
                 0,
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
