#ifndef __FOLDERSTATE_H_
#define __FOLDERSTATE_H_

typedef struct folderstate folderstate;

folderstate *folderstate_new(const char *folder);
void folderstate_destroy(folderstate *fs);
void folderstate_reload(folderstate *fs);

void folderstate_set_filter(folderstate *fs, const char *f);
const char *folderstate_get_filter(folderstate *fs);

int folderstate_get_nfiles(folderstate *);
int folderstate_get_state(folderstate *);
void folderstate_increase_state(folderstate *);
void folderstate_decrease_state(folderstate *);
const char *folderstate_get_path(folderstate *);
const char *folderstate_get_path_shortname(folderstate *);
const char **folderstate_get_files(folderstate *);

const char *folderstate_file_get_full_path_from_string(folderstate *fs, const char *s);
const char *folderstate_file_get_full_path_from_index(folderstate *fs, int index);

int folderstate_chdir(folderstate *, const char *);



#endif //_FOLDERSTATE_H_
