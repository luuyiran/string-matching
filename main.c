#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

typedef char* (*FUNC)(const char *hay, const char *needle);

typedef struct _APPROACH_ {
    const char *funcName;
    const FUNC func;
} APPROACH;

/* 根据时间消耗排序用 */
typedef struct _INFO_ {
    const char *funcName;
    const char *needle;
    size_t  count;
    clock_t clock;
} INFO;

/* 待测试函数的声明 */
extern char *naive(const char *haystack, const char *needle);
extern char *notNaive(const char *haystack, const char *needle);
extern char *kmp(const char *haystack, const char *needle);
extern char *bm(const char *haystack, const char *needle);
extern char *sunday(const char *haystack, const char *needle);
extern char *horspool(const char *haystack, const char *needle);
extern char *twoway(const char *haystack, const char *needle);
extern char *bitap(const char *text, const char *pattern);
extern char *strStr (const char *haystack, const char *needle);

/* 待搜索的模式串 */
static char *needles[] = {
    /* 针对 bible.txt */
    "And the God", 
    "the birth of",
    "good-for-nothing", 
    "and the Lord said", 
    /* 针对 aaa.txt */
    "aaaaaaaaaaaaaaaaaaaaaaaaa", 
    "aaaaaaaaaaaaaaaaaaaaaaaaV",
    "aaaaaaaaaaaaTaaaaaaaaaaaa",
    "Raaaaaaaaaaaaaaaaaaaaaaaa",
    /* 针对 abc.txt */
    "ABCDABD",
    "CDAB ABCDABCD"
};

/* 待测试函数 */
static APPROACH f[] = {
    {"naive",   naive},
    {"notNaive",notNaive},
    {"kmp",     kmp},
    {"bm",      bm},
    {"sunday",  sunday},
    {"horspool",horspool},
    {"twoway",  twoway},
    {"bitap",   bitap},
    {"strStr",  strStr},
    {"strstr",  strstr}
};


static void ptintTitle() {
    printf("\n       #                        words      frequency       time(cpu)\n");
    printf("--------------------------------------------------------------------\n");
}

static void print(INFO *info, size_t size) {
    size_t i;
    if (NULL == info || size <= 0) return;
    for (i = 0; i < size; i++)
        printf("%8s%28s%16ld%16ld\n", info[i].funcName, info[i].needle, info[i].count, info[i].clock);
    printf("--------------------------------------------------------------------\n");
}

/* 加载测试文本 */
static char *load(int argc, char *argv[], size_t *len) {
    char *buffer = NULL;
    size_t fileLen = 0;
    char *fileName = "bible.txt";  

    if (argc > 1) 
        fileName = argv[1];

    FILE *file = fopen(fileName, "rb");
    if (NULL == file) {
        printf("fopen %s error!\n", fileName);
        return NULL;
    }

    /* 获取文本长度 */
    fseek(file, 0, SEEK_END);
    fileLen = ftell(file);/* this is the number of bytes from the beginning of the file. */

    /* 分配内存，并初始化为'\0'，多分配一个'\0'作为结束标志 */
    buffer = (char *)calloc(fileLen + 1, sizeof(char));
    if (NULL == buffer) {
        printf("malloc buffer error! file:%s size:%lu\n", fileName, fileLen);
        fclose(file);
        return NULL;
    }

    /* 把测试文本加载到内存 */
    rewind(file); /* Set position of stream to the beginning */
    if (fread(buffer, 1, fileLen, file) != fileLen) {
        printf("load %s error!\n", fileName);
        fclose(file);
        free(buffer);
        return NULL;
    }

    printf(" load %s ok! %lu bytes\n", fileName, fileLen);

    fclose(file);
    *len = fileLen;
    return buffer;
}

static void drop(char *buffer) {
    if (buffer) 
        free(buffer);
}

static INFO frequency(APPROACH alg, const char *haystack, const char *needle) {
    INFO info;
    size_t count = 0;
    clock_t tic, toc;
    const char *end = haystack + strlen(haystack);

    tic = clock();
    while (haystack < end) {
        haystack = alg.func(haystack, needle);
        if (NULL == haystack) break;
        haystack += 1;
        count++;
    }
    toc = clock();

    info.funcName = alg.funcName;
    info.needle = needle;
    info.count = count;
    info.clock = toc - tic;
    return info;
}

static int cmpfunc(const void *a, const void *b){
    if (NULL == a || NULL == b) return 0;
   return ((INFO*)a)->clock - ((INFO*)b)->clock;
}

int main(int argc, char *argv[]) {
    size_t i, j, m, n;
    size_t len = 0;
    char *haystack = NULL;

    haystack = load(argc, argv, &len);
    if (NULL == haystack || 0 == len) 
        return -1;

    ptintTitle();

    m = ARRAY_SIZE(needles);
    n = ARRAY_SIZE(f);

    /* 同一个单词在不同算法下的查询时间排名 */
    for (i = 0; i < m; i++) { 
        INFO info[n];
        for (j = 0; j < n; j++)
            info[j] = frequency(f[j], haystack, needles[i]);
        qsort(info, n, sizeof(INFO), cmpfunc);
        print(info, n);
    }

    /* 同一个算法在查询不同单词下的时间消耗排名，突出其擅长查询的单词特征 */
    for (j = 0; j < n; j++) {
        INFO info[m];
        for (i = 0; i < m; i++) 
            info[i] = frequency(f[j], haystack, needles[i]);
        qsort(info, m, sizeof(INFO), cmpfunc);
        print(info, m);
    }

    drop(haystack);
    return 0;
}
