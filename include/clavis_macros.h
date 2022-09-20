#ifndef __CLAVIS_MACROS_H_
#define __CLAVIS_MACROS_H_

#include <file_io.h>
#include <gtk/gtk.h>

#ifdef __unix__
#define _(String) gettext(String)
#elif defined(_WIN32) || defined (WIN32)
#define _(String) windows_string(gettext(String))
#endif

#endif //_CLAVIS_MACROS_H_
