/* 
 * Knuth–Morris–Pratt algorithm， 1974
 *
 * References:
 * https://en.wikipedia.org/wiki/Knuth%E2%80%93Morris%E2%80%93Pratt_algorithm
 * http://www-igm.univ-mlv.fr/~lecroq/string/node8.html
 * https://www.ics.uci.edu/~eppstein/161/960227.html
 * https://www.zhihu.com/question/21923021
 * https://blog.csdn.net/v_july_v/article/details/7041827
 * https://citeseerx.ist.psu.edu/viewdoc/summary?doi=10.1.1.93.8147
 * http://jakeboxer.com/blog/2009/12/13/the-knuth-morris-pratt-algorithm-in-my-own-words/
 * Knuth, Donald; Morris, James H.; Pratt, Vaughan (1977). 
 * "Fast pattern matching in strings". SIAM Journal on Computing. 6 (2): 323–350.
 */

#include <stdio.h>
#include <string.h>

static void getNext(const char *needle, long long len, int *next);

char* kmp(const char *haystack, const char *needle) {
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

    int next[needleLen];

    getNext(needle, needleLen, next);

    while (i < haystackLen && j < needleLen) {
        if (j == -1 || haystack[i] == needle[j]) {
            i++;
            j++;
        } else 
            j = next[j];
    }

    if (j == needleLen)
        return (char *)&haystack[i - j];

    return NULL;
}

/* 版本三 */
static void getNext(const char *needle, long long len, int *next) {
    int i = 0;
    int j = -1;
    next[i] = j;
    while (i < len - 1) {
        if (j == -1 || needle[i] == needle[j]) {
            i++;
            j++;

            if (needle[i] != needle[j])
                next[i] = j;   /* 相同子串截至到 j 处 */
            else 
                next[i] = next[j]; /* 说明 i 处和 j 处的公共前缀一样，直接利用已求出的结果 */
        } else {
            j = next[j];
        }
    }
}

#if 0
/* 版本二  迭代 */
static void getNext(const char *needle, long long len, int *next) {
    int i = 0;
    int j = -1;
    next[i] = j;
    while (i < len - 1) {
        if (j == -1 || needle[i] == needle[j]) {
            i++, j++;
            next[i] = j;   /* 相同子串截至到 j 处 */
        } else 
            j = next[j];
    }
}

/* 版本一   暴力枚举*/
#define MAX(a, b) ((a)>(b)?(a):(b))
static void getNext(const char *needle, long long len, int *next) {
    int i, j, size, res;
    next[0] = 0;
    for (i = 1; i < len - 1; i++) {
        res = 0;
        /* size: 相等前后缀的最大长度, i+1个元素最大前缀长度 i */
        for (size = 1; size <= i; size++) {
            for (j = 0; j < size; j++) { /* j:前缀起始下标 */
                if (needle[j] != needle[i-size+1+j])
                    break;
            }
            if (j == size) {
                res = MAX(res, size);
            } 
        }
        next[i] = res;
    }

    /* 做平移操作 */
    for (i = len - 1; i >=1; i--)
        next[i] = next[i-1];
    next[0] = -1;
}

#endif 



