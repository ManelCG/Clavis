#include <clavis_regex.h>
#include <clavis_constants.h>
#include <unistd.h>
#include <stdio.h>

_Bool clavis_regex_git_url_is_https(const char *url){
  regex_t regex;
  int ret;

  const char *regex_str = CLAVIS_REGEX_HTTPS;

  ret = regcomp(&regex, regex_str, REG_EXTENDED);
  ret = regexec(&regex, url, 0, NULL, 0);
  if (!ret){
    printf("Match!\n");
    return true;
  } else if (ret == REG_NOMATCH){
    printf("No match\n");
    return false;
  } else {
    printf("Error\n");
    return false;
  }
}

_Bool clavis_regex_git_url_is_ssh(const char *url){
  regex_t regex;
  int ret;

  const char *regex_str = CLAVIS_REGEX_SSH;

  printf("Pre comp\n");
  ret = regcomp(&regex, regex_str, REG_EXTENDED);
  printf("Pre exec\n");
  ret = regexec(&regex, url, 0, NULL, 0);
  if (!ret){
    printf("Match!\n");
    return true;
  } else if (ret == REG_NOMATCH){
    printf("No match\n");
    return false;
  } else {
    printf("Error\n");
    return false;
  }
}
