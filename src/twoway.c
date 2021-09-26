/* Two-way string-matching algorithm, 1991
 * References:
 * http://www.quretec.com/u/vilo/edu/2002-03/Tekstialgoritmid_I/Articles/Exact/Two-way-p650-crochemore.pdf
 * http://www-igm.univ-mlv.fr/~lecroq/string/node26.html#SECTION00260
 * http://en.wikipedia.org/wiki/Boyer-Moore_string_search_algorithm
 * https://github.com/bminor/glibc/blob/master/string/str-two-way.h
 * 
 */

#include <string.h>
#include <limits.h>
#include <stdlib.h>

#define MAX(a, b) ((a)>(b)?(a):(b))

/* Computing of the maximal suffix for <= */
int maxSuf(const char *needle, int needleLen, int *p) {
    int ms, j, k;
    char a, b;

    ms = -1;
    j = 0;
    k = *p = 1;
    while (j + k < needleLen) {
        a = needle[j + k];
        b = needle[ms + k];
        if (a < b) {
            j += k;
            k = 1;
            *p = j - ms;
        } else if (a == b)
            if (k != *p)
                ++k;
            else {
                j += *p;
                k = 1;
            }
        else { /* a > b */
            ms = j;
            j = ms + 1;
            k = *p = 1;
        }
    }
    return (ms);
}

/* Computing of the maximal suffix for >= */
int maxSufTilde(const char *needle, int needleLen, int *p) {
    int ms, j, k;
    char a, b;

    ms = -1;
    j = 0;
    k = *p = 1;
    while (j + k < needleLen) {
        a = needle[j + k];
        b = needle[ms + k];
        if (a > b) {
            j += k;
            k = 1;
            *p = j - ms;
        } else if (a == b)
            if (k != *p)
                ++k;
            else {
                j += *p;
                k = 1;
            }
        else { /* a < b */
            ms = j;
            j = ms + 1;
            k = *p = 1;
        }
    }
    return (ms);
}

char *twoway(const char *haystack, const char *needle) {
    long long haystackLen = strlen(haystack);
    long long needleLen = strlen(needle);
    int i, j, ell, memory, p, per, q;

    /* Preprocessing */
    i = maxSuf(needle, needleLen, &p);
    j = maxSufTilde(needle, needleLen, &q);
    if (i > j) {
        ell = i;
        per = p;
    } else {
        ell = j;
        per = q;
    }

    /* Searching */
    if (memcmp(needle, needle + per, ell + 1) == 0) {
        j = 0;
        memory = -1;
        while (j <= haystackLen - needleLen) {
            i = MAX(ell, memory) + 1;
            while (i < needleLen && needle[i] == haystack[i + j]) ++i;
            if (i >= needleLen) {
                i = ell;
                while (i > memory && needle[i] == haystack[i + j]) --i;
                if (i <= memory) return (char *)&haystack[j];
                j += per;
                memory = needleLen - per - 1;
            } else {
                j += (i - ell);
                memory = -1;
            }
        }
    } else {
        per = MAX(ell + 1, needleLen - ell - 1) + 1;
        j = 0;
        while (j <= haystackLen - needleLen) {
            i = ell + 1;
            while (i < needleLen && needle[i] == haystack[i + j]) ++i;
            if (i >= needleLen) {
                i = ell;
                while (i >= 0 && needle[i] == haystack[i + j]) --i;
                if (i < 0) return (char *)&haystack[j];
                j += per;
            } else
                j += (i - ell);
        }
    }
    return NULL;
}

