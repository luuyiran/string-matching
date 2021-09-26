/*
 * not-so-naive Hancart, 1992
 * 
 * References:
 * https://www-igm.univ-mlv.fr/~lecroq/string/node13.html
 * HANCART C., 1992, Une analyse en moyenne de l'algorithme de Morris et Pratt et de ses raffinements, 
 * in Théorie des Automates et Applications, Actes des 2e Journées Franco-Belges, 
 * D. Krob ed., Rouen, France, 1991, PUR 176, Rouen, France, 99-110.
 */

#include <stdio.h>
#include <string.h>



char* notNaive(const char *haystack, const char *needle) {
    if (NULL == needle) 
        return NULL;
    if (NULL == haystack || '\0' == needle[0] || '\0' == needle[1]) 
        return (char *)haystack;

    size_t i, j;
    size_t haystackLen = strlen(haystack);
    size_t needleLen = strlen(needle);

    if (haystackLen < needleLen)
        return NULL;

    size_t equal = 2;
    size_t diff = 1;

    if (needle[0] == needle[1]) {
        equal = 1;
        diff = 2;
    }

    for (i = 0; i <= haystackLen - needleLen; ) {
        if (needle[1] != haystack[i+1]) 
            i += diff;
        else {
            for (j = 1; j < needleLen; j++)
                if (needle[j] != haystack[i+j])
                    break;
            if (j == needleLen && needle[0] == haystack[i])
                return (char *)&haystack[i];
            i += equal;
        }
    }
    return NULL;
}


