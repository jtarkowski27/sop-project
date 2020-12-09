
#include "regex_match.h"

int match(const char *string, char *pattern)
{
    int status;
    regex_t re;

    if (regcomp(&re, pattern, REG_EXTENDED|REG_NOSUB))
        ERR("regcomp");

    status = regexec(&re, string, (size_t) 0, NULL, 0);
    regfree(&re);

    if (status)
        return 0;

    return 1;
}