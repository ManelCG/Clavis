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
