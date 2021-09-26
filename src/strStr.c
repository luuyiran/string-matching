/* this file is strstr source code from glibc, 2008
 * Written by Eric Blake <ebb9@byu.net>, 2008.
 * copy from:
 * https://github.com/bminor/glibc/blob/master/string/str-two-way.h
 * https://github.com/bminor/glibc/blob/master/string/strstr.c
 * 
 * just for test with other string match algorithms.
 */

#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h> 

extern  size_t strnlen(const char *_Str, size_t _MaxCount);

#define RETURN_TYPE char *
#define AVAILABLE(h, h_l, j, n_l) \
    (((j) + (n_l) <= (h_l)) ||    \
     ((h_l) += strnlen((void *)((h) + (h_l)), 512), (j) + (n_l) <= (h_l)))
#define CHECK_EOL (1)
#define RET0_IF_0(a) \
    if (!a) goto ret0
#define FASTSEARCH(S, C, N) (void *)strchr((void *)(S), (C))

#define LONG_NEEDLE_THRESHOLD 32U

static size_t critical_factorization(const unsigned char *needle,
                                     size_t needle_len, size_t *period) {
    size_t max_suffix, max_suffix_rev;
    size_t j;           
    size_t k;           
    size_t p;           
    unsigned char a, b; 
    max_suffix = SIZE_MAX;
    j = 0;
    k = p = 1;
    while (j + k < needle_len) {
        a = (needle[j + k]);
        b = (needle[max_suffix + k]);
        if (a < b) {
            j += k;
            k = 1;
            p = j - max_suffix;
        } else if (a == b) {
            if (k != p)
                ++k;
            else {
                j += p;
                k = 1;
            }
        } else {
            max_suffix = j++;
            k = p = 1;
        }
    }
    *period = p;
    max_suffix_rev = SIZE_MAX;
    j = 0;
    k = p = 1;
    while (j + k < needle_len) {
        a = (needle[j + k]);
        b = (needle[max_suffix_rev + k]);
        if (b < a) {
            j += k;
            k = 1;
            p = j - max_suffix_rev;
        } else if (a == b) {
            if (k != p)
                ++k;
            else {
                j += p;
                k = 1;
            }
        } else 
        {
            max_suffix_rev = j++;
            k = p = 1;
        }
    }

    if (max_suffix_rev + 1 < max_suffix + 1) return max_suffix + 1;
    *period = p;
    return max_suffix_rev + 1;
}

static RETURN_TYPE two_way_short_needle(const unsigned char *haystack,
                                        size_t haystack_len,
                                        const unsigned char *needle,
                                        size_t needle_len) {
    size_t i;      /* Index into current byte of NEEDLE.  */
    size_t j;      /* Index into current window of HAYSTACK.  */
    size_t period; /* The period of the right half of needle.  */
    size_t suffix; /* The index of the right half of needle.  */

    suffix = critical_factorization(needle, needle_len, &period);

    if (memcmp(needle, needle + period, suffix) == 0) {
        size_t memory = 0;
        j = 0;
        while (AVAILABLE(haystack, haystack_len, j, needle_len)) {
            const unsigned char *pneedle;
            const unsigned char *phaystack;

            i = MAX(suffix, memory);
            pneedle = &needle[i];
            phaystack = &haystack[i + j];
            while (i < needle_len && ((*pneedle++) == (*phaystack++))) ++i;
            if (needle_len <= i) {
                i = suffix - 1;
                pneedle = &needle[i];
                phaystack = &haystack[i + j];
                while (memory < i + 1 && ((*pneedle--) == (*phaystack--))) --i;
                if (i + 1 < memory + 1) return (RETURN_TYPE)(haystack + j);
                j += period;
                memory = needle_len - period;
            } else {
                j += i - suffix + 1;
                memory = 0;
            }
        }
    } else {
        const unsigned char *phaystack;
        unsigned char needle_suffix = (needle[suffix]);

        period = MAX(suffix, needle_len - suffix) + 1;
        j = 0;
        while (AVAILABLE(haystack, haystack_len, j, needle_len)) {
            unsigned char haystack_char;
            const unsigned char *pneedle;

            phaystack = &haystack[suffix + j];

#ifdef FASTSEARCH
            if (*phaystack++ != needle_suffix) {
                phaystack = FASTSEARCH(phaystack, needle_suffix,
                                       haystack_len - needle_len - j);
                if (phaystack == NULL) goto ret0;
                j = phaystack - &haystack[suffix];
                phaystack++;
            }
#else
            while (needle_suffix != (haystack_char = (*phaystack++))) {
                RET0_IF_0(haystack_char);
#if !CHECK_EOL
                ++j;
                if (!AVAILABLE(haystack, haystack_len, j, needle_len))
                    goto ret0;
#endif
            }

#if CHECK_EOL
            j = phaystack - &haystack[suffix] - 1;
#endif
#endif
            i = suffix + 1;
            pneedle = &needle[i];
            while (i < needle_len) {
                if ((*pneedle++) != (haystack_char = (*phaystack++))) {
                    RET0_IF_0(haystack_char);
                    break;
                }
                ++i;
            }
#if CHECK_EOL
            if (phaystack > haystack + haystack_len)
                haystack_len = phaystack - haystack;
#endif
            if (needle_len <= i) {
                i = suffix - 1;
                pneedle = &needle[i];
                phaystack = &haystack[i + j];
                while (i != SIZE_MAX) {
                    if ((*pneedle--) != (haystack_char = (*phaystack--))) {
                        RET0_IF_0(haystack_char);
                        break;
                    }
                    --i;
                }
                if (i == SIZE_MAX) return (RETURN_TYPE)(haystack + j);
                j += period;
            } else
                j += i - suffix + 1;
        }
    }
ret0:
    __attribute__((unused)) return NULL;
}

static RETURN_TYPE two_way_long_needle(const unsigned char *haystack,
                                       size_t haystack_len,
                                       const unsigned char *needle,
                                       size_t needle_len) {
    size_t i;      /* Index into current byte of NEEDLE.  */
    size_t j;      /* Index into current window of HAYSTACK.  */
    size_t period; /* The period of the right half of needle.  */
    size_t suffix; /* The index of the right half of needle.  */
    size_t shift_table[1U << CHAR_BIT]; /* See below.  */

    suffix = critical_factorization(needle, needle_len, &period);

    for (i = 0; i < 1U << CHAR_BIT; i++) shift_table[i] = needle_len;
    for (i = 0; i < needle_len; i++)
        shift_table[(needle[i])] = needle_len - i - 1;

    if (memcmp(needle, needle + period, suffix) == 0) {
        size_t memory = 0;
        size_t shift;
        j = 0;
        while (AVAILABLE(haystack, haystack_len, j, needle_len)) {
            const unsigned char *pneedle;
            const unsigned char *phaystack;

            shift = shift_table[(haystack[j + needle_len - 1])];
            if (0 < shift) {
                if (memory && shift < period) {
                    shift = needle_len - period;
                }
                memory = 0;
                j += shift;
                continue;
            }

            i = MAX(suffix, memory);
            pneedle = &needle[i];
            phaystack = &haystack[i + j];
            while (i < needle_len - 1 && ((*pneedle++) == (*phaystack++))) ++i;
            if (needle_len - 1 <= i) {
                i = suffix - 1;
                pneedle = &needle[i];
                phaystack = &haystack[i + j];
                while (memory < i + 1 && ((*pneedle--) == (*phaystack--))) --i;
                if (i + 1 < memory + 1) return (RETURN_TYPE)(haystack + j);

                j += period;
                memory = needle_len - period;
            } else {
                j += i - suffix + 1;
                memory = 0;
            }
        }
    } else {
        size_t shift;
        period = MAX(suffix, needle_len - suffix) + 1;
        j = 0;
        while (AVAILABLE(haystack, haystack_len, j, needle_len)) {
            const unsigned char *pneedle;
            const unsigned char *phaystack;

            shift = shift_table[(haystack[j + needle_len - 1])];
            if (0 < shift) {
                j += shift;
                continue;
            }

            i = suffix;
            pneedle = &needle[i];
            phaystack = &haystack[i + j];
            while (i < needle_len - 1 && ((*pneedle++) == (*phaystack++))) ++i;
            if (needle_len - 1 <= i) {
                i = suffix - 1;
                pneedle = &needle[i];
                phaystack = &haystack[i + j];
                while (i != SIZE_MAX && ((*pneedle--) == (*phaystack--))) --i;
                if (i == SIZE_MAX) return (RETURN_TYPE)(haystack + j);
                j += period;
            } else
                j += i - suffix + 1;
        }
    }
    return NULL;
}

char *strStr(const char *haystack, const char *needle) {
    size_t needle_len;
    size_t haystack_len;
    if (needle[0] == '\0') return (char *)haystack;
    haystack = strchr(haystack, needle[0]);
    if (haystack == NULL || needle[1] == '\0') return (char *)haystack;
    needle_len = strlen(needle);
    haystack_len = strnlen(haystack, needle_len + 256);
    if (haystack_len < needle_len) return NULL;
    if (memcmp(haystack, needle, needle_len) == 0) return (char *)haystack;
    if (needle_len < LONG_NEEDLE_THRESHOLD)
        return two_way_short_needle((const unsigned char *)haystack,
                                    haystack_len, (const unsigned char *)needle,
                                    needle_len);
    return two_way_long_needle((const unsigned char *)haystack, haystack_len,
                               (const unsigned char *)needle, needle_len);
}

