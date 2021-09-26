/* 
 * Bitap algorithm, Bálint Dömölki, 1964
 * In 1991, it was extended by Manber and Wu to handle also insertions and deletions (full fuzzy string searching). 
 * This algorithm was later improved by Baeza-Yates and Navarro in 1996 and later by Gene Myers for long patterns in 1998.
 * 
*/

#include <stdio.h>
#include <string.h>

char *bitap(const char *text, const char *pattern) {
    int m = strlen(pattern);
    unsigned long R;
    unsigned long pattern_mask[128];
    int i;

    if (pattern[0] == '\0') return (char *)text;
    if (m > 31) {
        printf("The pattern is too long!\n");
        return "The pattern is too long!";
    }

    /* Initialize the bit array R */
    R = ~1;

    /* Initialize the pattern bitmasks */
    for (i = 0; i < 128; ++i) pattern_mask[i] = ~0;
    for (i = 0; i < m; ++i) pattern_mask[(size_t)pattern[i]] &= ~(1UL << i);

    for (i = 0; text[i] != '\0'; ++i) {
        /* Update the bit array */
        R |= pattern_mask[(size_t)text[i]];
        R <<= 1;

        if (0 == (R & (1UL << m))) return (char *)(text + i - m) + 1;
    }

    return NULL;
}


