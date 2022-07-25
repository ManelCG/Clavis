#ifndef __FILE_IO_H_
#define __FILE_IO_H_

_Bool file_io_string_is_folder(const char *s);
_Bool file_io_string_is_file(const char *s);

int file_io_folder_get_file_n(const char *folder);
char **file_io_folder_get_file_list(const char *folder, int nfiles);

const char *get_password_store_path();

#endif //_FILE_IO_H_
