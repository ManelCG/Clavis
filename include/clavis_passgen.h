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

#ifndef __CLAVIS_PASSGEN_H_
#define __CLAVIS_PASSGEN_H_

#include <stdbool.h>
#include <gtk/gtk.h>

typedef struct passgen passgen;

passgen *passgen_new();

void passgen_set_len(passgen *pg, int len);
void passgen_set_numerals(passgen *pg, _Bool numerals);
void passgen_set_symbols(passgen *pg, _Bool symbols);
void passgen_set_pronunceable(passgen *pg, _Bool pron);
void passgen_set_uppercase(passgen *pg, _Bool uppercase);
void passgen_set_lowercase(passgen *pg, _Bool lowercase);
void passgen_set_output_entry(passgen *pg, GtkWidget *);
int passgen_get_len(passgen *pg);
_Bool passgen_get_numerals(passgen *pg);
_Bool passgen_get_symbols(passgen *pg);
_Bool passgen_get_pronunceable(passgen *pg);
_Bool passgen_get_uppercase(passgen *pg);
_Bool passgen_get_lowercase(passgen *pg);
GtkWidget *passgen_get_output_entry(passgen *pg);

const char *passgen_generate_random_filename();

const char *passgen_generate_new_password(passgen *pg);


#endif //_CLAVIS_PASSGEN_H_
