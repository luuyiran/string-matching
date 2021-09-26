/* Horspool-Algorithmus, 1980
 *
 * References:
 * [Hor 80]	R.N. Horspool: Practical Fast Searching in Strings. Software - Practice and Experience 10, 501-506 (1980)
 * https://www.inf.fh-flensburg.de/lang/algorithmen/pattern/horse.htm
 * 
 */

#include <stdio.h>
#include <string.h>

#define CHARS 256
static void getShift(const char *needle, long long needleLen, int *shift);

char* horspool(const char *haystack, const char *needle) {
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
        j = needleLen - 1;
        while (j >= 0 && needle[j] == haystack[i + j]) 
            j--;

        if (j == -1)
            return (char *)&haystack[i];
        else {
            size_t str = haystack[i + needleLen - 1];
            i += shift[str];
        }
    }
    return NULL;
}


static void getShift(const char *needle, long long needleLen, int *shift) {
    int i;
    for (i = 0; i < CHARS; i++)
         shift[i] = needleLen;  /* 不在needle中则跨越 len+1 长度 */

    for (i = 0; i < needleLen - 1; i++) /* 最后一个作为比较，不予统计 */
         shift[(size_t)needle[i]] = needleLen - i - 1;
}
 

