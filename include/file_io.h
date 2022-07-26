#ifndef __FILE_IO_H_
#define __FILE_IO_H_

_Bool file_io_string_is_folder(const char *s);
_Bool file_io_string_is_file(const char *s);

_Bool file_io_rm_rf(const char *s);

int file_io_folder_get_file_n(const char *folder, const char *filter);
char **file_io_folder_get_file_list(const char *folder, int nfiles, const char *filter);

const char *file_io_decrypt_password(const char *path);

const char *get_password_store_path();

int mkdir_handler(const char *path);

#endif //_FILE_IO_H_
