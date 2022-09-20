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

#include <clavis_passgen.h>
#include <clavis_constants.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <openssl/rand.h>

typedef struct passgen{
  int len;
  _Bool numerals;
  _Bool symbols;
  _Bool pronunceable;
  _Bool uppercase;
  _Bool lowercase;
  GtkWidget *output_entry;
} passgen;

passgen *passgen_new(){
  passgen *pg = malloc(sizeof(passgen));
  pg->len = DEFAULT_PASSWORD_SIZE;
  pg->numerals = true;
  pg->symbols = false;
  pg->pronunceable = false;
  pg->uppercase = true;
  pg->lowercase = true;
  pg->output_entry = NULL;

  return pg;
}

const char *passgen_generate_new_password(passgen *pg){
  if (pg->pronunceable){
    //Generating charset
    int conslen = strlen(CLAVIS_PASSGEN_CONSONANTS_LOWERCASE)*pg->lowercase + strlen(CLAVIS_PASSGEN_CONSONANTS_UPPERCASE)*pg->uppercase;
    int vowelen = strlen(CLAVIS_PASSGEN_VOWELS_LOWERCASE)*pg->lowercase + strlen(CLAVIS_PASSGEN_VOWELS_UPPERCASE)*pg->uppercase;
    char consonants[conslen + 1];
    char vowels[vowelen + 1];
    _Bool added = false;
    if (pg->lowercase){
      strcpy(consonants, CLAVIS_PASSGEN_CONSONANTS_LOWERCASE);
      strcpy(vowels, CLAVIS_PASSGEN_VOWELS_LOWERCASE);
      added = true;
    }
    if (pg->uppercase){
      if (added){

        strcat(consonants, CLAVIS_PASSGEN_CONSONANTS_UPPERCASE);
        strcat(vowels, CLAVIS_PASSGEN_VOWELS_UPPERCASE);
      } else {
        strcpy(consonants, CLAVIS_PASSGEN_CONSONANTS_UPPERCASE);
        strcpy(vowels, CLAVIS_PASSGEN_VOWELS_UPPERCASE);
        added = true;
      }
    }
    if (! added){
      return NULL;
    }

    //Generate random password
    char *password = calloc(sizeof(char) * pg->len, 1);
    unsigned int randint;
    for (int i = 0; i < pg->len; i++){
      RAND_priv_bytes((unsigned char *) &randint, sizeof(randint));
      if (i % 2 == 0){
        randint = randint % conslen;
        password[i] = consonants[randint];
      } else {
        randint = randint % vowelen;
        password[i] = vowels[randint];
      }
    }

    if (pg->output_entry != NULL){
      gtk_entry_set_text(GTK_ENTRY(pg->output_entry), password);
    }
    return password;

  } else {
    int charslen = strlen(CLAVIS_PASSGEN_NUMERALS)*pg->numerals + (strlen(CLAVIS_PASSGEN_VOWELS_LOWERCASE)+strlen(CLAVIS_PASSGEN_CONSONANTS_LOWERCASE))*pg->lowercase + (strlen(CLAVIS_PASSGEN_VOWELS_UPPERCASE) + strlen(CLAVIS_PASSGEN_CONSONANTS_UPPERCASE))*pg->uppercase + strlen(CLAVIS_PASSGEN_SYMBOLS)*pg->symbols;
    char charlist[charslen+1];
    _Bool added = false;

    if (pg->numerals){
      strcpy(charlist, CLAVIS_PASSGEN_NUMERALS);
      added = true;
    }
    if (pg->symbols){
      if (added){
        strcat(charlist, CLAVIS_PASSGEN_SYMBOLS);
      } else {
        strcpy(charlist, CLAVIS_PASSGEN_SYMBOLS);
        added = true;
      }
    }
    if (pg->uppercase){
      if (added){
        strcat(charlist, CLAVIS_PASSGEN_CONSONANTS_UPPERCASE);
        strcat(charlist, CLAVIS_PASSGEN_VOWELS_UPPERCASE);
      } else {
        strcpy(charlist, CLAVIS_PASSGEN_CONSONANTS_UPPERCASE);
        strcat(charlist, CLAVIS_PASSGEN_VOWELS_UPPERCASE);
        added = true;
      }
    }
    if (pg->lowercase){
      if (added){
        strcat(charlist, CLAVIS_PASSGEN_CONSONANTS_LOWERCASE);
        strcat(charlist, CLAVIS_PASSGEN_VOWELS_LOWERCASE);
      } else {
        strcpy(charlist, CLAVIS_PASSGEN_CONSONANTS_LOWERCASE);
        strcat(charlist, CLAVIS_PASSGEN_VOWELS_LOWERCASE);
        added = true;
      }
    }
    if (! added){
      return NULL;
    }

    //Generate random password
    char *password = calloc(sizeof(char) * (pg->len + 1), 1);
    unsigned int randint;
    for (int i = 0; i < pg->len; i++){
      RAND_priv_bytes((unsigned char *) &randint, sizeof(randint));
      randint = randint % charslen;
      password[i] = charlist[randint];
    }

    password[pg->len] = '\0';

    if (pg->output_entry != NULL){
      gtk_entry_set_text(GTK_ENTRY(pg->output_entry), password);
    }
    return password;
  }
}

void passgen_set_len(passgen *pg, int len){
  pg->len = len;
}
void passgen_set_numerals(passgen *pg, _Bool numerals){
  pg->numerals = numerals;
}
void passgen_set_symbols(passgen *pg, _Bool symbols){
  pg->symbols = symbols;
}
void passgen_set_pronunceable(passgen *pg, _Bool pron){
  pg->pronunceable = pron;
}
void passgen_set_uppercase(passgen *pg, _Bool uppercase){
  pg->uppercase = uppercase;
}
void passgen_set_lowercase(passgen *pg, _Bool lowercase){
  pg->lowercase = lowercase;
}
void passgen_set_output_entry(passgen *pg, GtkWidget *w){
  pg->output_entry = w;
}

int passgen_get_len(passgen *pg){
  return pg->len;
}
_Bool passgen_get_numerals(passgen *pg){
  return pg->numerals;
}
_Bool passgen_get_symbols(passgen *pg){
  return pg->symbols;
}
_Bool passgen_get_pronunceable(passgen *pg){
  return pg->pronunceable;
}
_Bool passgen_get_uppercase(passgen *pg){
  return pg->uppercase;
}
_Bool passgen_get_lowercase(passgen *pg){
  return pg->lowercase;
}
GtkWidget *passgen_get_output_entry(passgen *pg){
  return pg->output_entry;
}
