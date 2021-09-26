#include <stdio.h>
#include <string.h>

/*
 * https://www.geeksforgeeks.org/naive-algorithm-for-pattern-searching/
 */
char* naive(const char *haystack, const char *needle) {
    if (NULL == needle) 
        return NULL;
    if (NULL == haystack || '\0' == needle[0] || '\0' == needle[1]) 
        return (char *)haystack;

    size_t i, j;
    size_t haystackLen = strlen(haystack);
    size_t needleLen = strlen(needle);

    if (haystackLen < needleLen)
        return NULL;

    for (i = 0; i <= haystackLen - needleLen; i++) {
        for (j = 0; j < needleLen; j++) {
            if (needle[j] != haystack[i+j])
                break;
        }
        if (j == needleLen)
            return (char *)&haystack[i];
    }
    return NULL;
}

/**
 * 最佳情况 O(N)： 模式串 needle 的第一个字母根本没有出现在文本 haystack 中
 * 
 * 最坏情况：O(m*(n-m+1))
 * 1.所有字符相同，当搜索txt中全部的part时
 * txt[] = "AAAAAAAAAAAAAAAAAA";
 * pat[] = "AAAAA";
 * 
 * 2.只有最后一个字符不同
 * txt[] = "AAAAAAAAAAAAAAAAAB";
 * pat[] = "AAAAB";
 * **/
