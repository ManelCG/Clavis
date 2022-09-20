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

#ifndef __FILE_IO_H_
#define __FILE_IO_H_

_Bool file_io_string_is_folder(const char *s);
_Bool file_io_string_is_file(const char *s);

_Bool file_io_rm_rf(const char *s);
int file_io_get_file_count(const char *s, _Bool recursive);

int file_io_folder_get_file_n(const char *folder, const char *filter);
char **file_io_folder_get_file_list(const char *folder, int nfiles, const char *filter);
_Bool file_io_folder_has_gpgid(const char *path);

const char *file_io_decrypt_password(const char *path);
int file_io_encrypt_password(const char *password, const char *path);

const char *get_password_store_path();

int mkdir_handler(const char *path);
int cp(const char *from, const char *to);

const char *file_io_get_git_config_field(const char *field);
int file_io_init_git_server(const char *username, const char *email, const char *repo_url, _Bool createnew, _Bool refactor_git);
int file_io_get_git_auth_method();
int file_io_get_git_auth_method_from_string(const char *s);
void perform_git_command(char *args);


char **file_io_get_full_gpg_keys(int *num);
char **file_io_get_gpg_keys(int *num, _Bool secret);

void file_io_export_gpg_keys(const char *key, const char *path, _Bool private);
void file_io_gpg_trust_key(const char *key);
void file_io_init_password_store(const char *key);

char *file_io_get_gpgid();

int file_io_remove_password(const char *path);

char *file_io_get_clavis_executable();
char *file_io_get_clavis_folder();
char *file_io_get_clavis_license_file_buffer();

#ifdef __unix__
#ifndef MAKE_INSTALL
char *file_io_get_about_picture_png();
#endif
#endif
#if defined(_WIN32) || defined (WIN32)
char *file_io_get_about_picture_png();
char *file_io_get_gtk_settings_ini_file();
int file_io_get_gtk_theme();
char *windows_string(const char *s);
#endif

#endif //_FILE_IO_H_
