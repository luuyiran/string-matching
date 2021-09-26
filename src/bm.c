/*
 * Boyer–Moore string-search algorithm, 1977 
 *
 * References:
 * https://www.geeksforgeeks.org/boyer-moore-algorithm-for-pattern-searching/
 * https://www.inf.hs-flensburg.de/lang/algorithmen/pattern/bmen.htm
 * https://doi.org/10.1145%2F359842.359859
 * Boyer, Robert S.; Moore, J Strother (October 1977). "A Fast String Searching Algorithm". 
 * Comm. ACM. New York: Association for Computing Machinery. 20 (10): 762–772. 
 */


#include <stdio.h>
#include <string.h>

#define CHARS 256
#define MAX(a, b) ((a)>(b)?(a):(b))

static void bmInitocc(const char *needle, long long needleLen, int *badchar);
static void bmPreprocess1(const char *needle, long long needleLen, int *bpos, int *shift);
static void bmPreprocess2(long long needleLen, int *bpos, int *shift);

char* bm(const char *haystack, const char *needle) {
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

    int badchar[CHARS];
    int bpos[needleLen + 1];
    int shift[needleLen + 1];
    memset(shift, 0, sizeof(shift));
    bmInitocc(needle, needleLen, badchar);
    bmPreprocess1(needle, needleLen, bpos, shift);
    bmPreprocess2(needleLen, bpos, shift);

    while (i <= haystackLen - needleLen) {
        j = needleLen - 1;
        while (j >= 0 && needle[j] == haystack[i + j]) 
            j--;
        if (j == -1)
            return (char *)&haystack[i];
        else 
            i += MAX(shift[j + 1], j - badchar[(size_t)haystack[i + j]]);
            /* The pattern is shifted by the longest of the two distances */ 
            /* that are given by the bad character and the good suffix heuristics. */
    }

    return NULL;
}

/**
 * Bad character heuristics method
 */
static void bmInitocc(const char *needle, long long needleLen, int *badchar) {
    int i;
    for (i = 0; i < CHARS; i++)
         badchar[i] = -1;

    for (i = 0; i < needleLen; i++)
         badchar[(size_t)needle[i]] = i;
}
 
/**
 * Good suffix heuristics method
 */
static void bmPreprocess1(const char *needle, long long needleLen, int *bpos, int *shift) {
    int i = needleLen, j = needleLen + 1;
    bpos[i] = j;
    while (i > 0) {
        while (j <= needleLen && needle[i-1] != needle[j-1]) {
            if (shift[j] == 0) 
                shift[j] = j-i;
            j = bpos[j];
        }
        i--; j--;
        bpos[i] = j;
    }
}

static void bmPreprocess2(long long needleLen, int *bpos, int *shift) {
    int i, j;
    j = bpos[0];
    for (i = 0; i <= needleLen; i++) {
        if (shift[i] == 0) 
            shift[i] = j;
        if (i == j) 
            j = bpos[j];
    }
}


