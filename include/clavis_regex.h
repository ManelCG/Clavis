#ifndef __CLAVIS_REGEX_H_
#define __CLAVIS_REGEX_H_

#include <regex.h>
#include <stdbool.h>

_Bool clavis_regex_git_url_is_https(const char *url);
_Bool clavis_regex_git_url_is_ssh(const char *url);


#endif //_CLAVIS_REGEX_H_
