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

const char *passgen_generate_new_password(passgen *pg);


#endif //_CLAVIS_PASSGEN_H_
