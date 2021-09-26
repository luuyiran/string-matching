/* Sunday algorithm, 1990
 * References:
 * https://www.inf.fh-flensburg.de/lang/algorithmen/pattern/sundayen.htm
 * [Sun 90]	D.M. Sunday: A Very Fast Substring Search Algorithm. Communications of the ACM, 33, 8, 132-142 (1990)
 * 
 */

#include <stdio.h>
#include <string.h>

#define CHARS 256
static void getShift(const char *needle, long long needleLen, int *shift);


char* sunday(const char *haystack, const char *needle) {
    if (NULL == needle) 
        return NULL;
    if (NULL == haystack || '\0' == needle[0] || '\0' == needle[1]) 
        return (char *)haystack;

    int i = 0;
    int j = 0;
    long long haystackLen = strlen(haystack);
    long long needleLen = strlen(needle);

    if (haystackLen < needleLen)
        return NULL;

    int shift[CHARS];
    getShift(needle, needleLen, shift);

    while (i <= haystackLen - needleLen) {
        j = 0;
        while (j < needleLen && needle[j] == haystack[i + j]) 
            j++;
        if (j == needleLen)
            return (char *)&haystack[i];
        else {
            size_t str = haystack[i + needleLen];
            i += shift[str];
        }
    }
    return NULL;
}


static void getShift(const char *needle, long long needleLen, int *shift) {
    int i;
    for (i = 0; i < CHARS; i++)
         shift[i] = needleLen + 1;  /* 不在needle中则跨越 len+1 长度 */

    for (i = 0; i < needleLen; i++)
         shift[(size_t)needle[i]] = needleLen - i;
}

