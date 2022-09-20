#ifndef __CLAVIS_MACROS_H_
#define __CLAVIS_MACROS_H_

#include <file_io.h>
#include <gtk/gtk.h>

#ifdef __unix__
#define _(String) gettext(String)
#elif defined(_WIN32) || defined (WIN32)
//windows_string() is defined in src/file_io.c and changes the format of string
//From single-char utf-8 special characters to wide multi-byte characters
//that windows understands
#define _(String) windows_string(gettext(String))
#endif

#endif //_CLAVIS_MACROS_H_
