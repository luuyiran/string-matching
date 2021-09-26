字符串匹配
====================

本文选取了8种字符串比较算法(`naive`,`not-so-naive`,`KMP`,`Boyer-Moore`,`Horspool`,`Sunday`,`two-way,bitap`)，实现接口参考C库函数`strstr`：

```c++
/* Return the first occurrence of NEEDLE in HAYSTACK.  Return HAYSTACK
   if NEEDLE is empty, otherwise NULL if NEEDLE is not found in
   HAYSTACK.  */
char *
STRSTR (const char *haystack, const char *needle);
```

着重讲解了`KMP`算法，最后统一测试比较性能。


描述复杂度时，`N`表示`haystack`长度，`M`表示`needle`长度，且`N`远大于`M`。


描述算法时，`haystackLen`表示文本串`haystack`长度，`needleLen`表示模式串`needle`长度。`i`用来表示文本串`haystack`的下标，`j`用来表示模式串`needle`的下标。

#### 一、暴力匹配
从头到尾依次比较，把模式串比作一个窗口的话，这个窗口从文本串的下标`0`开始滑动，若不匹配则搜索位置`+1`滑动一下。

![brute](https://img-blog.csdnimg.cn/20210716094924426.png#pic_center)


```c++
for (i = 0; i <= haystackLen - needleLen; i++) {
    for (j = 0; j < needleLen; j++) {
        if (needle[j] != haystack[i+j])
            break;
    }
    if (j == needleLen)
        return &haystack[i];
}
return NULL;
```
复杂度分析 ：

最佳情况 `O(N)`：

`needle` 的第一个字母根本没有出现在文本 `haystack` 中，最后返回`NULL`.

最坏情况：`O(M*N)`

1. 所有字符相同，当搜索haystack中全部的part时
```
haystack[] = "AAAAAAAAAAAAAAAAAA";
needle[] = "AAAAA";
```
2. 只有最后一个字符不同
```
haystack[] = "AAAAAAAAAAAAAAAAAB";
needle[] = "AAAAB";
```

#### 二、not-so-naive

对暴力匹配算法做出了改进，某些条件下允许滑动`2`个位置。

该算法从needle的第二个字符开始比较，直到结尾，最后比较第一个字符。

程序比较前，记录下needle前两个字符是否相等的信息，在`needle[0] == needle[1]`的情况下，如果`needle[1] != haystack[i+1]`就给文本索引`i`增加`2`，跳过前两个字符，进行下一次循环。如果`needle[1] == haystack[i+1]`就退化成了暴力匹配，窗口比较完成后，向后滑动1个位置.

```c++
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
            for (j = 1; j < needleLen; j++) {
                if (needle[j] != haystack[i+j])
                    break;
            }
            if (j == needleLen && needle[0] == haystack[i])
                return &haystack[i];
            i += equal;
        }
    }
    return NULL;
```
复杂度分析：

最佳`O(N/2)` .

最坏`O(M*N)`.

#### 三、Knuth–Morris–Pratt

needle字符串中每个位置可能有与前缀相同的部分，如下图中的`ABC`，当遇到失配位置`g`-`D`时，利用已经比较完毕的`ABC`的信息，needle直接滑动到到与`g`-`D`前面`ABC`对齐的位置，继续`g`-`X`处的比较.

![kmp](https://img-blog.csdnimg.cn/2021071610284066.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L01DXzAwNw==,size_16,color_FFFFFF,t_70#pic_center)

整个过程文本串`haystack`没有回溯动作，下标`i`是持续增长的，needle的下标`j`当遇到失配情况时是有回溯动作的，回溯到哪个位置，根据该位置处的前面几个字符与needle起始处的字符信息来确定，我们举例来说明，如上图，当haystack[i]在g处needle[j]在D处时，g-D不匹配，我们根据D处前面的字符信息ABC和needle开头ABC相同的信息，我们发现，ABC已经匹配完毕了，没必要从头继续匹配，继续从ABC的下一个字符X处比较，我们管这个动作叫next，用于寻找下一个开始匹配的下标位置：next[D]->x, 

```c++
    i = 0, j = 0;
    while (i < haystackLen && j < needleLen) {
        if (haystack[i] == needle[j]) // ABC匹配上
            i++, j++;
        else 
            j = next[j];  // g - D失配，下标由D->X处的下标
    }

    if (j == needleLen)
        return &haystack[i - j];

    return NULL;
```

接下来我们解决next数组问题，为了描述，我们引入前缀后缀的概念，例如ABCDABC：
```
前缀不包含最后一个字符，前缀子串为：
A
AB
ABC
ABCD
ABCDA
ABCDAB
后缀不包含第一个字符，后缀子串为：
      C
     BC
    ABC
   DABC
  CDABC
 BCDABC
```
前后缀里面相等的最长子串为ABC，ABC即为最长相等前后缀.

现在我们以上图的`ABCXABCD`作为needle为例，我们j从0开始，截取[0-j]位置上的子串，依次求每个子串的最长相等前后缀及其长度，

j   |  子串   | 前缀 | 后缀 | 最长相等前后缀  | 长度
----|---------|-----|------| ---            | ----
0   | A       | \   | \    | 无  | 0
1   | AB      | A   | B    | 无  | 0
2   | ABC     | A,AB  | BC,C | 无  | 0
3   | ABCX    | A,AB,ABC  | BCX,CX,X    | 无  | 0
4   | ABCXA   | A,AB,ABC,ABCX  | BCXA,CXA,XA,A    | A   | 1
5   | ABCXAB  | A,AB,ABC,ABCX,ABCXA  | BCXAB,CXAB,XAB,AB,B    | AB  | 2
6   | ABCXABC | A,AB,ABC,ABCX,ABCXA,ABCXAB  | BCXABC,CXABC,XABC,ABC,BC,C    | ABC | 3
7   | ABCXABCD| A,AB,ABC,ABCX,ABCXA,ABCXAB,ABCXABC  | BCXABCD,CXABCD,XABCD,ABCD,BCD,CD,D    | 无  | 0

把最长相等前后缀长度填入下表：
needle |A|B|C|X|A|B|C|D
-------|-|-|-|-|-|-|-|--
j      |0|1|2|3|4|5|6|7
长度   |0|0|0|0|1|2|3|0


X继续和haystack中的i比较，

如下图，现在我们把下标和最长相等前后缀长度标上，当`j=7`时`D`-`g`失配，根据上面的讨论，此时neddle应该滑动到前缀ABC对齐处，对齐之后，j的下标跑到了`前缀`ABC之后的X处，j的下标此时从`7`变为了`3`，这个`3`恰好是模式串失配时上一个位置`j=6`时的最长相等前后缀，我们要越过这个相等前缀长度`3`，那下一个位置的下标自然是这个`3`，可能有点绕，我们还是以`ABC`前缀举例，它的长度为3，他的下一个位置下标索引即是`3`.

在这个过程中我们也发现`j=7`失配时，我们要找`j=6`的位置，刚好错位，每次失配都要访问前面元素的最长相等前后缀，为了处理方便，我们长度数组整体向右滑动一位，next[7]对齐到`3`，失配时即可以直接`j=next[7]`=>`3`回溯过去，继续从`j=3`处比较，`i`不动。



![123](https://img-blog.csdnimg.cn/20210716131238836.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L01DXzAwNw==,size_16,color_FFFFFF,t_70#pic_center)


整体往右挪一个位置，起始位置设为-1.

![kkk](https://img-blog.csdnimg.cn/20210716131238867.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L01DXzAwNw==,size_16,color_FFFFFF,t_70#pic_center)

当j==-1时，表示我们回溯到了起始位置处，失配位置的地方与needle第一个字符肯定不相等，因为没有公共相等前后缀，此时i向后移动做自增操作i++，j也自增j++，继续下一个比较，这时j刚好为0，从开头处，继续匹配。因此上面的代码稍作修改：

```c++
    i = 0, j = 0;
    while (i < haystackLen && j < needleLen) {
        if (j == -1 || haystack[i] == needle[j])
            i++, j++;
        else 
            j = next[j];
    }

    if (j == needleLen)
        return &haystack[i - j];

    return NULL;
```

我们继续以模式串`ABCXABCD`为例，通过右移，初始位置初始化为-1得到next数组：
needle |A|B|C|X|A|B|C|D
-------|-|-|-|-|-|-|-|--
j      |0|1|2|3|4|5|6|7
next   |-1|0|0|0|0|1|2|3

现在我们已经搞清楚`next`是怎么回事了：依次求取每个位置上的最长相等前后缀，之后数组向后平移一个位置。我们首先想到的就是暴力枚举，对于`needle`的每个位置`i`，枚举所有长度前后缀，并比较是否相等，相等的话如果是当前最大则保存这个长度，最后我们在位置i处会得到一个长度的最大值，这个最大值就是最长相等前后缀的长度，即`next[i]`的值。求完所有next[i]后，我们做一个右移操作。

```c++
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
```
我们不满足于暴力求解，现在我们用递推求取next数组，初始时：
```c++
j = -1;
i = 0;
next[i] = j;
```
next[0] = -1;即初始位置；

由于平移操作,next[1]总是等于0，这一步可用 j == -1来判断。

next[1] = 0;

我们用i来遍历needle，i指向前一个位置的最长相等前后缀的next值，下标j表示模式串needle的前缀[0-j]，j是有可能不停回溯的，失配时，逐渐缩短前缀长度比较。

假如`前缀`现在走到了`j`，`后缀`走到了`i`，走到目前表示j个前缀字符已和i之前的j个字符匹配上，

此时如果needle[j] == needle[i]，可不就是最长相等前后缀都扩展了一个长度吗？

由于错位关系，这个长度不能赋值到下标i处，要赋值到下一处，因j是下标，所以长度要加1，即next[i+1] = j+1;

如下图：

![kmp3](https://img-blog.csdnimg.cn/20210716132023213.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L01DXzAwNw==,size_16,color_FFFFFF,t_70#pic_center)


接下来比较`i`和`j`的下一位，此时`j`到了`X`字符处，`i`到了`D`字符处，此时失配，失配时怎么办呢？我们只有缩短前缀来继续比较，缩短前缀的办法就是求取删除`j`后的`ABC`的最长相等前后缀，因为错位关系，`next[j]`即是`ABC`的最长相等前后缀，如果找到这个最长相等前后缀后(`j`已经next[`j`]之后)，我们继续比较，尝试扩展长度，即下一层的`needle[i] == needle[j]`，如果未找到，j将会回溯到-1，表示最长相等前后缀为0，i和j都向后挪一位，新的i从needle的下标0处继续扩展前后缀。

```c++
static void Next(const char *needle, long long len, int *next) {
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
```

#### 四、Boyer-Moore
该算法从needle的最后一个字符开始比较，失配时根据坏字符规则和好后缀原则向后偏移。

##### 1.坏字符规则：

当haystack中的字符失配时，寻找该失配处的字符在needle中最靠右的位置，并向右滑动needle使该字符对齐，若该字符不存在，i直接跳过needleLen长度进行下一处比较。

如下图，当`i = 2`时，从后面开始向前匹配，首先匹配`i=7`处，`C`和`G`失配，并且`C`不在needle中，于是`i`跨过整个`needle`长度，跳到 `i` `+=` `needleLength` --> `8` 处，然后继续从13下标处继续比较.

![bad](https://media.geeksforgeeks.org/wp-content/uploads/bad_match_heuristic_case_2.jpg)

因此我们先建立一个查询表`badchar`，表示每个字符在最右的下标，用`-1`表示没有出现。

```c++
static void bmInitocc(const char *needle, int needleLen, int *badchar) {
    int i;
    for (i = 0; i < CHARS; i++)
         badchar[i] = -1;
    for (i = 0; i < needleLen; i++)
         badchar[needle[i]] = i;
}
```

有了这个表，我们可以写出如下程序
```c++
    i = 0;
    while (i <= haystackLen - needleLen) {
        j = needleLen - 1;  // 从最后一个字符比较
        while (j >= 0 && needle[j] == haystack[i + j]) 
            j--;   //从后向前比较
        if (j == -1)
            return &haystack[i];
        else 
            i += j - badchar[haystack[i + j]];
    }
    return NULL;
```

因为当前haystack失配符有可能在needle对应字符的左边，所以有可能产生负号，所以代码修正为：

```c++
    i += MAX(1, j - badchar[haystack[i + j]]);
```

##### 2.好后缀规则：

好后缀分两种情况：

case 1：匹配的后缀同时出现在needle其他地方。


![bm](https://www.inf.hs-flensburg.de/lang/algorithmen/pattern/rand6.gif)

case 2: 匹配的后缀包含needle起始的部分。

![bm1](https://www.inf.hs-flensburg.de/lang/algorithmen/pattern/rand7.gif)

例1：case 1 情况
```c
0	1	2	3	4	5	6	7	8	9	...
a	b	a	a	b	a	b	a	c	b	a
c	a	b	a	b						
		c	a	b	a	b				
```
后缀ab已匹配上，2处发生失配，后缀ab也存在于前面，因此，向后滑动对齐。

例2：
```c
0	1	2	3	4	5	6	7	8	9	...
a	b	c	a	b	a	b	a	c	b	a
c	b	a	a	b
					c	b	a	a	b	
```
后缀ab已匹配上，2处发生失配，后缀ab没有在needle前面出现，因此，跨过整个needle长度.

例3：case 2 情况
```
0	1	2	3	4	5	6	7	8	9	...
a	a	b	a	b	a	b	a	c	b	a
a	b	b	a	b			
			a	b	b	a	b		
```
后缀bab已经匹配，1处发生失配，bab在needle前部没有出现，但此时不能平移到5处，因为前缀`ab`在`bab`中发生匹配，因此平移到`3`处：`ab`前缀对齐处.

我们用`shift[i]`表示`i-1`处失配时，需要平移的距离.

`bpos[i]`表示从`i`处的公共前缀后缀的位置。

例如从i后的子串为babab,最长公共前缀后缀为bab，bab起始处为 bpos[i] = i+2;

case 1情况和`KMP`预处理情况类似，`KMP`是寻找每个位置和前缀相等的最大长度，现在的情况只是反过来，后缀不断增长，寻求前面与后缀相等的子串，bpos数组类似KMP中的next数组，

```c++
static void bmPreprocess1(const char *needle, int needleLen, int *bpos, int *shift) {
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
```

case 2的情况，

```c++
static void bmPreprocess2(int needleLen, int *bpos, int *shift) {
    int i, j;
    j = bpos[0];
    for (i = 0; i <= needleLen; i++) {
        if (shift[i] == 0) 
            shift[i] = j;
        if (i == j) 
            j = bpos[j];
    }
}
```

#### 五、Horspool
Horspool算法可以从任意位置开始匹配.当发生失配时，查找haystack的最后一个字符在needle中的最右位置，并对齐。若不存在，i直接跨国needleLen长度。

例如：

```c++
0	1	2	3	4	5	6	7	8	9	...
a	b	c	a	b	d	a	a	c	b	a
b	c	a	a	b
				b	c	a	a	b	
```

建立字符索引，顺便计算距离结尾距离，作为偏移，注意，最后一个字符不要统计，不然有可能会陷入死循环，因为当最后一个字符相等时，它将不会移动，因为它自己就是最右一个字符，无需平移！
```c++
static void getShift(const char *needle, int needleLen, int *shift) {
    int i;
    for (i = 0; i < CHARS; i++)
         shift[i] = needleLen;  /* 不在needle中则跨越 len+1 长度 */

    for (i = 0; i < needleLen - 1; i++) /* 最后一个作为比较，不予统计 */
         shift[(size_t)needle[i]] = needleLen - i - 1;
}
```

算法如下：
```c++
    i = 0;
    while (i <= haystackLen - needleLen) {
        j = needleLen - 1;  // 暂且从后开始比较
        while (j >= 0 && needle[j] == haystack[i + j]) 
            j--;

        if (j == -1)
            return &haystack[i];
        else {
            size_t str = haystack[i + needleLen - 1];
            i += shift[str];
        }
    }
    return NULL;
```
#### 六、Sunday

Sunday算法也可以从任意位置开始匹配，为了提高效率可以根据字符概率来排序匹配顺序，当失配时，直接比较窗口右边外的第一个字符，若存在于needle中，则使之对齐。

不存在时，跨越`needleLen + 1`长度.

例如`2`处 `c-a`失配，则查询`5`处`d`在needle中的情况.
```c++
0	1	2	3	4	5	6	7	8	9	...
a	b	c	a	b	d	a	a	c	b	a
b	c	a	a	b
						b	c	a	a	b
```

因此我们首先建立一个对齐距离映射表.
```c++
static void getShift(const char *needle, int needleLen, int *shift) {
    int i;
    for (i = 0; i < CHARS; i++)
         shift[i] = needleLen + 1;  /* 不在needle中则跨越 len+1 长度 */

    for (i = 0; i < needleLen; i++)
         shift[(size_t)needle[i]] = needleLen - i;  // 对齐距离
}
```

主函数代码如下，每次遇到不匹配的字符后 查找最后外的字符haystack[i + needleLen]在needle中的对齐距离.
```c++
    i = 0;
    while (i <= haystackLen - needleLen) {
        j = 0;
        while (j < needleLen && needle[j] == haystack[i + j]) 
            j++;
        if (j == needleLen)
            return &haystack[i];
        else {
            size_t str = haystack[i + needleLen];
            i += shift[str];
        }
    }
    return NULL;
```

总结：`Boyer-Moore`失配时搜索`失配处`的字符，`Horspool`失配时搜索`最后一个`字符，`Sunday`搜索`下一个窗口`起始位置的字符.

```c++
// Boyer-Moore
0	1	2	3	4	5	6	7	8	9	...
a	b	c	a	b	d	a	a	c	b	a
b	c	a	a	b					
	b	c	a	a	b	
```

```c++
// Horspool
0	1	2	3	4	5	6	7	8	9	...
a	b	c	a	b	d	a	a	c	b	a
b	c	a	a	b	
				b	c	a	a	b		
```

```c++
// Sunday
0	1	2	3	4	5	6	7	8	9	...
a	b	c	a	b	d	a	a	c	b	a
b	c	a	a	b
						b	c	a	a	b
```

#### 七、two-way

glibc中的strstr实现方式。

双向字符串匹配算法，可以看作是前向`KMP`和后向`Boyer-Moore`算法的组合，它使用常数空间，时间复杂度最坏O(2MN).

该算法在实际条件下相当有效，对缓存友好，被选为很多库的实现方式(glibc的strstr).

#### 八、bitap

Bitap算法（或称为shift-or、shift-and、Baeza-Yates–Gonnet算法）是一种字符串近似匹配算法。该算法可判断给定的文本是否包含与定义模式“近似相等”的子字符串。其中，根据莱文斯坦距离 – 如果子字符串和定义模式彼此在给定距离“K”之内，则该算法认为他们近似。该算法预先计算一组位掩码，其中每个位掩码的每一个元素都包含一个模式。

#### 九、strstr

glibc中的strstr采用two-way算法实现，它保证了在常数空间下线性时间复杂度。

strstr在调用two-way之前，算法首先找到第一个字符匹配的位置，未找到直接返回NULL.

```c++
  /* Skip until we find the first matching char from NEEDLE.  */
  haystack = strchr (haystack, needle[0]);
  if (haystack == NULL || needle[1] == '\0')
    return (char *) haystack;
```

之后获取needle长度和haystack预处理缓存长度
```c++
  /* Ensure HAYSTACK length is at least as long as NEEDLE length.
     Since a match may occur early on in a huge HAYSTACK, use strnlen
     and read ahead a few cachelines for improved performance.  */
  needle_len = strlen (needle);
  haystack_len = __strnlen (haystack, needle_len + 256);
  if (haystack_len < needle_len)
    return NULL;
```



找到之后先进行一次memcmp判断是否相等，若是恰好找到，直接返回该位置。
```c++
  /* Check whether we have a match.  This improves performance since we avoid
     the initialization overhead of the two-way algorithm.  */
  if (memcmp (haystack, needle, needle_len) == 0)
    return (char *) haystack;
```
之后再根据needle长短选择two-way算法分支，对于大于32字节的长字符串，会建立一个类似Boyer-Moore算法的坏字符偏移表来提高性能。
```c++
  /* Perform the search.  Abstract memory is considered to be an array
     of 'unsigned char' values, not an array of 'char' values.  See
     ISO C 99 section 6.2.6.1.  */
  if (needle_len < LONG_NEEDLE_THRESHOLD)
    return two_way_short_needle ((const unsigned char *) haystack,
				 haystack_len,
				 (const unsigned char *) needle, needle_len);
  return two_way_long_needle ((const unsigned char *) haystack, haystack_len,
			      (const unsigned char *) needle, needle_len);
```

具体实现参考[src/strStr.c](src/strStr.c)


#### 十、测试

测试环境：


<table>
    <tr>
        <td> OS</td>>
        <td> ubuntu 18.04.1</td>>
        <td> Linux 5.4.0</td>>
    </tr>
    <tr>
        <td>gcc</td> 
        <td>7.5.0</td> 
        <td> </td> 
    </tr>
    <tr>
        <td>glibc</td> 
        <td>2.27</td> 
        <td>stable release version</td> 
    </tr>

</table>



测试方法：

构建一个needle列表、测试函数列表，交叉比对模式串的匹配时间。

按消耗时间打印测试结果。


测试步骤：

1.编译

```c++
admin@ubuntu:~$ make
gcc -o test src/strStr.c src/sunday.c src/bm.c src/twoway.c src/kmp.c \
src/naive.c src/bitap.c src/not-naive.c src/horspool.c main.c \
-std=c89 -W -Wall -g
```

2.测试`bible.txt`

着重测试常规的一些短语。

`strstr`一骑绝尘，sunday、horspool、bitap、bm均表现不错，KMP全部垫底.

```c++
admin@ubuntu:~$ ./test
 load bible.txt ok! 4467663 bytes

       #                        words      frequency       time(cpu)
--------------------------------------------------------------------
  strstr                 And the God               2             500
  sunday                 And the God               2            2435
   bitap                 And the God               2            2865
      bm                 And the God               2            2929
   naive                 And the God               2            3254
horspool                 And the God               2            3509
notNaive                 And the God               2            4730
  strStr                 And the God               2            9420
  twoway                 And the God               2           10249
     kmp                 And the God               2           10317
--------------------------------------------------------------------
  strstr                the birth of              23            1631
   bitap                the birth of              23            2471
horspool                the birth of              23            4079
  sunday                the birth of              23            4151
      bm                the birth of              23            4649
notNaive                the birth of              23            7530
   naive                the birth of              23            8410
  strStr                the birth of              23            8729
  twoway                the birth of              23           13114
     kmp                the birth of              23           14982
--------------------------------------------------------------------
  strstr            good-for-nothing              22             503
   bitap            good-for-nothing              22            2532
horspool            good-for-nothing              22            2739
      bm            good-for-nothing              22            3082
  sunday            good-for-nothing              22            3714
   naive            good-for-nothing              22            4588
  strStr            good-for-nothing              22            4762
notNaive            good-for-nothing              22            5824
  twoway            good-for-nothing              22            7483
     kmp            good-for-nothing              22           11716
--------------------------------------------------------------------
  strstr           and the Lord said              11            1070
  sunday           and the Lord said              11            2507
   bitap           and the Lord said              11            2613
horspool           and the Lord said              11            2836
      bm           and the Lord said              11            2989
notNaive           and the Lord said              11            5927
  strStr           and the Lord said              11            8336
   naive           and the Lord said              11            8589
  twoway           and the Lord said              11           10125
     kmp           and the Lord said              11           13183
--------------------------------------------------------------------
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaa               0             511
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaa               0             819
horspool   aaaaaaaaaaaaaaaaaaaaaaaaa               0             904
      bm   aaaaaaaaaaaaaaaaaaaaaaaaa               0            1149
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaa               0            2311
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaa               0            2645
   naive   aaaaaaaaaaaaaaaaaaaaaaaaa               0            5468
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaa               0           10214
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaa               0           11500
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaa               0           11730
--------------------------------------------------------------------
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaV               0             281
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaV               0             305
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaV               0             793
      bm   aaaaaaaaaaaaaaaaaaaaaaaaV               0            1060
horspool   aaaaaaaaaaaaaaaaaaaaaaaaV               0            1099
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2329
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2977
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaV               0            4913
   naive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            5513
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaV               0           12298
--------------------------------------------------------------------
  strstr   aaaaaaaaaaaaTaaaaaaaaaaaa               0             400
horspool   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1083
  sunday   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1375
      bm   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1698
   bitap   aaaaaaaaaaaaTaaaaaaaaaaaa               0            2496
notNaive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            3001
   naive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            5944
  twoway   aaaaaaaaaaaaTaaaaaaaaaaaa               0            7301
  strStr   aaaaaaaaaaaaTaaaaaaaaaaaa               0            9016
     kmp   aaaaaaaaaaaaTaaaaaaaaaaaa               0           11707
--------------------------------------------------------------------
  strstr   Raaaaaaaaaaaaaaaaaaaaaaaa               0             339
  sunday   Raaaaaaaaaaaaaaaaaaaaaaaa               0             838
horspool   Raaaaaaaaaaaaaaaaaaaaaaaa               0             864
      bm   Raaaaaaaaaaaaaaaaaaaaaaaa               0            1135
   naive   Raaaaaaaaaaaaaaaaaaaaaaaa               0            2581
   bitap   Raaaaaaaaaaaaaaaaaaaaaaaa               0            3324
  strStr   Raaaaaaaaaaaaaaaaaaaaaaaa               0            4047
notNaive   Raaaaaaaaaaaaaaaaaaaaaaaa               0            4463
  twoway   Raaaaaaaaaaaaaaaaaaaaaaaa               0            5950
     kmp   Raaaaaaaaaaaaaaaaaaaaaaaa               0           10402
--------------------------------------------------------------------
  strStr                     ABCDABD               0             198
  strstr                     ABCDABD               0             218
notNaive                     ABCDABD               0            1311
   bitap                     ABCDABD               0            2583
  sunday                     ABCDABD               0            2587
   naive                     ABCDABD               0            2716
horspool                     ABCDABD               0            3008
      bm                     ABCDABD               0            3422
  twoway                     ABCDABD               0            3773
     kmp                     ABCDABD               0           10057
--------------------------------------------------------------------
  strStr               CDAB ABCDABCD               0             326
  strstr               CDAB ABCDABCD               0             956
  sunday               CDAB ABCDABCD               0            1355
horspool               CDAB ABCDABCD               0            1572
notNaive               CDAB ABCDABCD               0            2089
      bm               CDAB ABCDABCD               0            2210
   naive               CDAB ABCDABCD               0            2471
   bitap               CDAB ABCDABCD               0            2631
  twoway               CDAB ABCDABCD               0            3621
     kmp               CDAB ABCDABCD               0           10997
--------------------------------------------------------------------
   naive               CDAB ABCDABCD               0            2510
   naive                     ABCDABD               0            3020
   naive                 And the God               2            3674
   naive   Raaaaaaaaaaaaaaaaaaaaaaaa               0            4038
   naive            good-for-nothing              22            4568
   naive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            5662
   naive   aaaaaaaaaaaaaaaaaaaaaaaaa               0            6505
   naive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            7026
   naive           and the Lord said              11            7058
   naive                the birth of              23           11484
--------------------------------------------------------------------
notNaive               CDAB ABCDABCD               0            1428
notNaive                     ABCDABD               0            2164
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaa               0            2656
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2919
notNaive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            3493
notNaive                 And the God               2            4904
notNaive   Raaaaaaaaaaaaaaaaaaaaaaaa               0            5070
notNaive           and the Lord said              11            5398
notNaive            good-for-nothing              22            6460
notNaive                the birth of              23            7117
--------------------------------------------------------------------
     kmp               CDAB ABCDABCD               0            9607
     kmp   Raaaaaaaaaaaaaaaaaaaaaaaa               0            9609
     kmp                     ABCDABD               0            9815
     kmp                 And the God               2           10288
     kmp   aaaaaaaaaaaaTaaaaaaaaaaaa               0           11042
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaa               0           11075
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaV               0           11190
     kmp            good-for-nothing              22           12182
     kmp           and the Lord said              11           13053
     kmp                the birth of              23           17945
--------------------------------------------------------------------
      bm   aaaaaaaaaaaaaaaaaaaaaaaaV               0             905
      bm   Raaaaaaaaaaaaaaaaaaaaaaaa               0             933
      bm   aaaaaaaaaaaaTaaaaaaaaaaaa               0             954
      bm   aaaaaaaaaaaaaaaaaaaaaaaaa               0             981
      bm               CDAB ABCDABCD               0            2048
      bm                 And the God               2            2846
      bm                     ABCDABD               0            2909
      bm            good-for-nothing              22            2948
      bm           and the Lord said              11            3812
      bm                the birth of              23            4396
--------------------------------------------------------------------
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaV               0             816
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaa               0             848
  sunday   aaaaaaaaaaaaTaaaaaaaaaaaa               0             864
  sunday   Raaaaaaaaaaaaaaaaaaaaaaaa               0            1198
  sunday               CDAB ABCDABCD               0            1594
  sunday                     ABCDABD               0            2188
  sunday           and the Lord said              11            2644
  sunday                 And the God               2            2970
  sunday            good-for-nothing              22            3863
  sunday                the birth of              23            8733
--------------------------------------------------------------------
horspool   aaaaaaaaaaaaaaaaaaaaaaaaV               0             793
horspool   aaaaaaaaaaaaaaaaaaaaaaaaa               0             829
horspool   aaaaaaaaaaaaTaaaaaaaaaaaa               0             884
horspool   Raaaaaaaaaaaaaaaaaaaaaaaa               0             973
horspool               CDAB ABCDABCD               0            1944
horspool            good-for-nothing              22            2587
horspool           and the Lord said              11            2610
horspool                     ABCDABD               0            3141
horspool                 And the God               2            3329
horspool                the birth of              23            6313
--------------------------------------------------------------------
  twoway                     ABCDABD               0            3630
  twoway               CDAB ABCDABCD               0            3671
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaV               0            4159
  twoway   Raaaaaaaaaaaaaaaaaaaaaaaa               0            5500
  twoway   aaaaaaaaaaaaTaaaaaaaaaaaa               0            7708
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaa               0            9987
  twoway            good-for-nothing              22            9993
  twoway                 And the God               2           11329
  twoway           and the Lord said              11           12234
  twoway                the birth of              23           12240
--------------------------------------------------------------------
   bitap            good-for-nothing              22            2445
   bitap                the birth of              23            2473
   bitap                 And the God               2            2509
   bitap   aaaaaaaaaaaaTaaaaaaaaaaaa               0            2531
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2588
   bitap   Raaaaaaaaaaaaaaaaaaaaaaaa               0            2606
   bitap           and the Lord said              11            2609
   bitap                     ABCDABD               0            2880
   bitap               CDAB ABCDABCD               0            2934
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaa               0            3240
--------------------------------------------------------------------
  strStr               CDAB ABCDABCD               0             162
  strStr                     ABCDABD               0             179
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaV               0             320
  strStr   Raaaaaaaaaaaaaaaaaaaaaaaa               0            3333
  strStr            good-for-nothing              22            4866
  strStr                the birth of              23            8857
  strStr           and the Lord said              11            9334
  strStr   aaaaaaaaaaaaTaaaaaaaaaaaa               0            9496
  strStr                 And the God               2            9741
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaa               0           11050
--------------------------------------------------------------------
  strstr               CDAB ABCDABCD               0             217
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaa               0             276
  strstr                     ABCDABD               0             278
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaV               0             346
  strstr   aaaaaaaaaaaaTaaaaaaaaaaaa               0             379
  strstr                 And the God               2             399
  strstr   Raaaaaaaaaaaaaaaaaaaaaaaa               0             485
  strstr            good-for-nothing              22             693
  strstr           and the Lord said              11            1191
  strstr                the birth of              23            2942
--------------------------------------------------------------------
```


3.测试`aaa.txt`

aaa.txt中字符均为'a',着重比较最坏情况“aaaaaaaaaaaaaaaaaaaaaaaaa”等极端情况的查找。当模式串都相同的时候，只有strstr、bitap胜出，其他几个时间不相上下，因为此时时间复杂度都为最差。

当只有其中一个字符不同时，bm，two-way表现不错。

KMP这次测试勉强及格吧。

```c
admin@ubuntu:~$ ./test aaa.txt 
 load aaa.txt ok! 639070 bytes

       #                        words      frequency       time(cpu)
--------------------------------------------------------------------
  strStr                 And the God               0              33
  strstr                 And the God               0              46
notNaive                 And the God               0             291
horspool                 And the God               0             345
  sunday                 And the God               0             360
      bm                 And the God               0             398
   bitap                 And the God               0             524
   naive                 And the God               0             634
  twoway                 And the God               0             717
     kmp                 And the God               0            2040
--------------------------------------------------------------------
  strStr                the birth of               0              18
  strstr                the birth of               0              31
horspool                the birth of               0             221
      bm                the birth of               0             292
notNaive                the birth of               0             323
  sunday                the birth of               0             349
   bitap                the birth of               0             377
  twoway                the birth of               0             605
   naive                the birth of               0             620
     kmp                the birth of               0            1764
--------------------------------------------------------------------
  strStr            good-for-nothing               0              34
  strstr            good-for-nothing               0              49
  sunday            good-for-nothing               0             137
horspool            good-for-nothing               0             152
notNaive            good-for-nothing               0             228
      bm            good-for-nothing               0             228
   naive            good-for-nothing               0             369
   bitap            good-for-nothing               0             392
  twoway            good-for-nothing               0             662
     kmp            good-for-nothing               0            1522
--------------------------------------------------------------------
  strStr           and the Lord said               0              25
  strstr           and the Lord said               0              74
notNaive           and the Lord said               0             244
   bitap           and the Lord said               0             368
  twoway           and the Lord said               0             631
  sunday           and the Lord said               0             758
   naive           and the Lord said               0             990
horspool           and the Lord said               0            1376
      bm           and the Lord said               0            1550
     kmp           and the Lord said               0            1581
--------------------------------------------------------------------
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaa          639046           11311
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaa          639046           11415
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaa          639046           45283
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2888026
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2909376
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2923815
horspool   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2957565
   naive   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2971096
      bm   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2976840
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         3091629
--------------------------------------------------------------------
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaV               0              19
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaV               0             483
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaV               0             758
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaV               0            1472
horspool   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2832
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2839
      bm   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2926
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaV               0            3437
   naive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            6323
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            9867
--------------------------------------------------------------------
   bitap   aaaaaaaaaaaaTaaaaaaaaaaaa               0             367
      bm   aaaaaaaaaaaaTaaaaaaaaaaaa               0             424
  twoway   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1137
  strStr   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1271
     kmp   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1772
  strstr   aaaaaaaaaaaaTaaaaaaaaaaaa               0            2967
   naive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            3435
horspool   aaaaaaaaaaaaTaaaaaaaaaaaa               0            4387
  sunday   aaaaaaaaaaaaTaaaaaaaaaaaa               0            4421
notNaive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            5250
--------------------------------------------------------------------
  strStr   Raaaaaaaaaaaaaaaaaaaaaaaa               0              19
  strstr   Raaaaaaaaaaaaaaaaaaaaaaaa               0              28
      bm   Raaaaaaaaaaaaaaaaaaaaaaaa               0             408
   bitap   Raaaaaaaaaaaaaaaaaaaaaaaa               0             440
   naive   Raaaaaaaaaaaaaaaaaaaaaaaa               0             441
  twoway   Raaaaaaaaaaaaaaaaaaaaaaaa               0             470
     kmp   Raaaaaaaaaaaaaaaaaaaaaaaa               0            1556
  sunday   Raaaaaaaaaaaaaaaaaaaaaaaa               0            2378
notNaive   Raaaaaaaaaaaaaaaaaaaaaaaa               0            4735
horspool   Raaaaaaaaaaaaaaaaaaaaaaaa               0            6725
--------------------------------------------------------------------
  strStr                     ABCDABD               0              15
  strstr                     ABCDABD               0              29
notNaive                     ABCDABD               0             173
   naive                     ABCDABD               0             332
   bitap                     ABCDABD               0             412
horspool                     ABCDABD               0             441
      bm                     ABCDABD               0             505
  twoway                     ABCDABD               0             534
  sunday                     ABCDABD               0             643
     kmp                     ABCDABD               0            1493
--------------------------------------------------------------------
  strStr               CDAB ABCDABCD               0              15
  strstr               CDAB ABCDABCD               0              30
notNaive               CDAB ABCDABCD               0             172
  sunday               CDAB ABCDABCD               0             177
horspool               CDAB ABCDABCD               0             201
      bm               CDAB ABCDABCD               0             236
   bitap               CDAB ABCDABCD               0             368
   naive               CDAB ABCDABCD               0             422
  twoway               CDAB ABCDABCD               0             532
     kmp               CDAB ABCDABCD               0            1534
--------------------------------------------------------------------
   naive                the birth of               0             359
   naive                 And the God               0             361
   naive            good-for-nothing               0             361
   naive               CDAB ABCDABCD               0             411
   naive                     ABCDABD               0             430
   naive   Raaaaaaaaaaaaaaaaaaaaaaaa               0             521
   naive           and the Lord said               0             863
   naive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            3401
   naive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            5953
   naive   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         3016905
--------------------------------------------------------------------
notNaive               CDAB ABCDABCD               0             172
notNaive                     ABCDABD               0             177
notNaive                the birth of               0             188
notNaive                 And the God               0             190
notNaive           and the Lord said               0             231
notNaive            good-for-nothing               0             249
notNaive   Raaaaaaaaaaaaaaaaaaaaaaaa               0            4262
notNaive   aaaaaaaaaaaaTaaaaaaaaaaaa               0            4760
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaV               0            8868
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2979533
--------------------------------------------------------------------
     kmp               CDAB ABCDABCD               0            1343
     kmp   Raaaaaaaaaaaaaaaaaaaaaaaa               0            1344
     kmp                the birth of               0            1350
     kmp            good-for-nothing               0            1350
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaV               0            1392
     kmp           and the Lord said               0            1410
     kmp                     ABCDABD               0            1424
     kmp                 And the God               0            1425
     kmp   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1453
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         2956387
--------------------------------------------------------------------
      bm            good-for-nothing               0             176
      bm               CDAB ABCDABCD               0             213
      bm                the birth of               0             232
      bm                 And the God               0             291
      bm   Raaaaaaaaaaaaaaaaaaaaaaaa               0             320
      bm                     ABCDABD               0             403
      bm   aaaaaaaaaaaaTaaaaaaaaaaaa               0             423
      bm           and the Lord said               0            1351
      bm   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2899
      bm   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         3042354
--------------------------------------------------------------------
  sunday            good-for-nothing               0             138
  sunday                the birth of               0             175
  sunday               CDAB ABCDABCD               0             211
  sunday                 And the God               0             251
  sunday                     ABCDABD               0             307
  sunday           and the Lord said               0             787
  sunday   Raaaaaaaaaaaaaaaaaaaaaaaa               0            2449
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaV               0            3404
  sunday   aaaaaaaaaaaaTaaaaaaaaaaaa               0            4302
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         3124617
--------------------------------------------------------------------
horspool            good-for-nothing               0             164
horspool               CDAB ABCDABCD               0             194
horspool                the birth of               0             214
horspool                 And the God               0             233
horspool                     ABCDABD               0             420
horspool           and the Lord said               0            1291
horspool   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2578
horspool   aaaaaaaaaaaaTaaaaaaaaaaaa               0            4025
horspool   Raaaaaaaaaaaaaaaaaaaaaaaa               0            6328
horspool   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         3052916
--------------------------------------------------------------------
  twoway   Raaaaaaaaaaaaaaaaaaaaaaaa               0             367
  twoway            good-for-nothing               0             517
  twoway               CDAB ABCDABCD               0             517
  twoway                the birth of               0             552
  twoway           and the Lord said               0             554
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaV               0             559
  twoway                 And the God               0             587
  twoway                     ABCDABD               0             589
  twoway   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1058
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaa          639046         3081626
--------------------------------------------------------------------
   bitap                the birth of               0             357
   bitap           and the Lord said               0             357
   bitap                 And the God               0             358
   bitap            good-for-nothing               0             359
   bitap                     ABCDABD               0             366
   bitap   aaaaaaaaaaaaTaaaaaaaaaaaa               0             367
   bitap   Raaaaaaaaaaaaaaaaaaaaaaaa               0             368
   bitap               CDAB ABCDABCD               0             369
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaV               0             373
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaa          639046           44864
--------------------------------------------------------------------
  strStr                the birth of               0              14
  strStr   Raaaaaaaaaaaaaaaaaaaaaaaa               0              14
  strStr            good-for-nothing               0              15
  strStr           and the Lord said               0              15
  strStr                     ABCDABD               0              15
  strStr                 And the God               0              16
  strStr               CDAB ABCDABCD               0              16
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaV               0              30
  strStr   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1279
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaa          639046           11063
--------------------------------------------------------------------
  strstr                     ABCDABD               0              29
  strstr            good-for-nothing               0              30
  strstr   Raaaaaaaaaaaaaaaaaaaaaaaa               0              30
  strstr               CDAB ABCDABCD               0              30
  strstr                 And the God               0              31
  strstr           and the Lord said               0              32
  strstr                the birth of               0              33
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2737
  strstr   aaaaaaaaaaaaTaaaaaaaaaaaa               0            2945
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaa          639046           11593
--------------------------------------------------------------------
```


4.测试`abc.txt`

着重测试有前后缀的情况，着重比较"ABCDABD""CDAB ABCDABCD"的查询效果，strstr、bitap、two-way表现不错，bm垫底，看来bm不擅长处理有较多重复的模式串。KMP表现仍然很差。

```c++
admin@ubuntu:~ $ ./test abc.txt 
 load abc.txt ok! 1163086 bytes

       #                        words      frequency       time(cpu)
--------------------------------------------------------------------
  strstr                 And the God               0             160
notNaive                 And the God               0             375
  sunday                 And the God               0             638
   bitap                 And the God               0             701
      bm                 And the God               0             838
horspool                 And the God               0             857
  twoway                 And the God               0            1074
  strStr                 And the God               0            1111
   naive                 And the God               0            1234
     kmp                 And the God               0            2947
--------------------------------------------------------------------
  strStr                the birth of               0              57
  strstr                the birth of               0              95
notNaive                the birth of               0             347
  sunday                the birth of               0             374
      bm                the birth of               0             539
   bitap                the birth of               0             716
horspool                the birth of               0             745
   naive                the birth of               0             775
  twoway                the birth of               0            1327
     kmp                the birth of               0            2727
--------------------------------------------------------------------
  strStr            good-for-nothing               0              26
  strstr            good-for-nothing               0              53
  sunday            good-for-nothing               0             274
horspool            good-for-nothing               0             355
notNaive            good-for-nothing               0             391
      bm            good-for-nothing               0             618
   bitap            good-for-nothing               0             704
   naive            good-for-nothing               0             732
  twoway            good-for-nothing               0            1052
     kmp            good-for-nothing               0            2844
--------------------------------------------------------------------
  strStr           and the Lord said               0              73
  strstr           and the Lord said               0              82
notNaive           and the Lord said               0             313
      bm           and the Lord said               0             356
  sunday           and the Lord said               0             389
horspool           and the Lord said               0             419
   bitap           and the Lord said               0             655
   naive           and the Lord said               0             687
  twoway           and the Lord said               0            1090
     kmp           and the Lord said               0            2588
--------------------------------------------------------------------
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaa               0              52
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaa               0             179
horspool   aaaaaaaaaaaaaaaaaaaaaaaaa               0             209
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaa               0             215
      bm   aaaaaaaaaaaaaaaaaaaaaaaaa               0             263
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaa               0             327
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaa               0             705
   naive   aaaaaaaaaaaaaaaaaaaaaaaaa               0             840
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaa               0            1995
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaa               0            2742
--------------------------------------------------------------------
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaV               0              23
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaV               0              50
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaV               0             196
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaV               0             204
      bm   aaaaaaaaaaaaaaaaaaaaaaaaV               0             255
horspool   aaaaaaaaaaaaaaaaaaaaaaaaV               0             302
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaV               0             650
   naive   aaaaaaaaaaaaaaaaaaaaaaaaV               0             771
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaV               0             939
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2734
--------------------------------------------------------------------
  strStr   aaaaaaaaaaaaTaaaaaaaaaaaa               0              24
  strstr   aaaaaaaaaaaaTaaaaaaaaaaaa               0              49
  sunday   aaaaaaaaaaaaTaaaaaaaaaaaa               0             170
horspool   aaaaaaaaaaaaTaaaaaaaaaaaa               0             209
      bm   aaaaaaaaaaaaTaaaaaaaaaaaa               0             223
notNaive   aaaaaaaaaaaaTaaaaaaaaaaaa               0             266
   naive   aaaaaaaaaaaaTaaaaaaaaaaaa               0             606
   bitap   aaaaaaaaaaaaTaaaaaaaaaaaa               0             651
  twoway   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1899
     kmp   aaaaaaaaaaaaTaaaaaaaaaaaa               0            2470
--------------------------------------------------------------------
  strstr   Raaaaaaaaaaaaaaaaaaaaaaaa               0              57
  strStr   Raaaaaaaaaaaaaaaaaaaaaaaa               0             126
  sunday   Raaaaaaaaaaaaaaaaaaaaaaaa               0             171
horspool   Raaaaaaaaaaaaaaaaaaaaaaaa               0             187
      bm   Raaaaaaaaaaaaaaaaaaaaaaaa               0             355
notNaive   Raaaaaaaaaaaaaaaaaaaaaaaa               0             383
   naive   Raaaaaaaaaaaaaaaaaaaaaaaa               0             664
   bitap   Raaaaaaaaaaaaaaaaaaaaaaaa               0             688
  twoway   Raaaaaaaaaaaaaaaaaaaaaaaa               0            1088
     kmp   Raaaaaaaaaaaaaaaaaaaaaaaa               0            2996
--------------------------------------------------------------------
  strstr                     ABCDABD           50569            1137
   bitap                     ABCDABD           50569            2026
  strStr                     ABCDABD           50569            2814
  twoway                     ABCDABD           50569          448362
horspool                     ABCDABD           50569          450065
  sunday                     ABCDABD           50569          453777
   naive                     ABCDABD           50569          464810
notNaive                     ABCDABD           50569          474200
     kmp                     ABCDABD           50569          474309
      bm                     ABCDABD           50569          477305
--------------------------------------------------------------------
  strstr               CDAB ABCDABCD           50569             793
   bitap               CDAB ABCDABCD           50569            2437
  strStr               CDAB ABCDABCD           50569            4000
  twoway               CDAB ABCDABCD           50569          448751
notNaive               CDAB ABCDABCD           50569          448891
     kmp               CDAB ABCDABCD           50569          449078
  sunday               CDAB ABCDABCD           50569          449435
horspool               CDAB ABCDABCD           50569          450804
   naive               CDAB ABCDABCD           50569          453527
      bm               CDAB ABCDABCD           50569          455962
--------------------------------------------------------------------
   naive   aaaaaaaaaaaaaaaaaaaaaaaaa               0             602
   naive   aaaaaaaaaaaaaaaaaaaaaaaaV               0             604
   naive           and the Lord said               0             634
   naive   Raaaaaaaaaaaaaaaaaaaaaaaa               0             635
   naive   aaaaaaaaaaaaTaaaaaaaaaaaa               0             722
   naive            good-for-nothing               0             818
   naive                the birth of               0             997
   naive                 And the God               0            1208
   naive                     ABCDABD           50569          452021
   naive               CDAB ABCDABCD           50569          452342
--------------------------------------------------------------------
notNaive   aaaaaaaaaaaaTaaaaaaaaaaaa               0             166
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaV               0             169
notNaive   aaaaaaaaaaaaaaaaaaaaaaaaa               0             175
notNaive            good-for-nothing               0             311
notNaive           and the Lord said               0             313
notNaive                the birth of               0             317
notNaive   Raaaaaaaaaaaaaaaaaaaaaaaa               0             351
notNaive                 And the God               0             367
notNaive                     ABCDABD           50569          466479
notNaive               CDAB ABCDABCD           50569          472021
--------------------------------------------------------------------
     kmp           and the Lord said               0            2479
     kmp   aaaaaaaaaaaaTaaaaaaaaaaaa               0            2481
     kmp            good-for-nothing               0            2507
     kmp                the birth of               0            2548
     kmp   Raaaaaaaaaaaaaaaaaaaaaaaa               0            2590
     kmp                 And the God               0            2649
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaV               0            2732
     kmp   aaaaaaaaaaaaaaaaaaaaaaaaa               0            2969
     kmp               CDAB ABCDABCD           50569          476538
     kmp                     ABCDABD           50569          479010
--------------------------------------------------------------------
      bm   aaaaaaaaaaaaaaaaaaaaaaaaV               0             213
      bm   aaaaaaaaaaaaTaaaaaaaaaaaa               0             214
      bm   Raaaaaaaaaaaaaaaaaaaaaaaa               0             215
      bm   aaaaaaaaaaaaaaaaaaaaaaaaa               0             248
      bm           and the Lord said               0             316
      bm            good-for-nothing               0             427
      bm                the birth of               0             463
      bm                 And the God               0             628
      bm               CDAB ABCDABCD           50569          477425
      bm                     ABCDABD           50569          480785
--------------------------------------------------------------------
  sunday   aaaaaaaaaaaaTaaaaaaaaaaaa               0             170
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaV               0             172
  sunday   aaaaaaaaaaaaaaaaaaaaaaaaa               0             206
  sunday   Raaaaaaaaaaaaaaaaaaaaaaaa               0             233
  sunday            good-for-nothing               0             313
  sunday                the birth of               0             346
  sunday           and the Lord said               0             391
  sunday                 And the God               0             471
  sunday               CDAB ABCDABCD           50569          475996
  sunday                     ABCDABD           50569          478272
--------------------------------------------------------------------
horspool   aaaaaaaaaaaaaaaaaaaaaaaaa               0             185
horspool   aaaaaaaaaaaaTaaaaaaaaaaaa               0             186
horspool   aaaaaaaaaaaaaaaaaaaaaaaaV               0             187
horspool   Raaaaaaaaaaaaaaaaaaaaaaaa               0             218
horspool           and the Lord said               0             274
horspool            good-for-nothing               0             312
horspool                the birth of               0             398
horspool                 And the God               0             538
horspool               CDAB ABCDABCD           50569          476582
horspool                     ABCDABD           50569          477463
--------------------------------------------------------------------
  twoway            good-for-nothing               0             929
  twoway           and the Lord said               0             929
  twoway                the birth of               0             963
  twoway   Raaaaaaaaaaaaaaaaaaaaaaaa               0             966
  twoway                 And the God               0            1001
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaV               0            1004
  twoway   aaaaaaaaaaaaaaaaaaaaaaaaa               0            1863
  twoway   aaaaaaaaaaaaTaaaaaaaaaaaa               0            1923
  twoway               CDAB ABCDABCD           50569          477191
  twoway                     ABCDABD           50569          481597
--------------------------------------------------------------------
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaV               0             618
   bitap   aaaaaaaaaaaaTaaaaaaaaaaaa               0             619
   bitap                the birth of               0             622
   bitap                 And the God               0             624
   bitap   aaaaaaaaaaaaaaaaaaaaaaaaa               0             650
   bitap            good-for-nothing               0             651
   bitap   Raaaaaaaaaaaaaaaaaaaaaaaa               0             651
   bitap           and the Lord said               0             680
   bitap                     ABCDABD           50569            1984
   bitap               CDAB ABCDABCD           50569            2426
--------------------------------------------------------------------
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaa               0              25
  strStr   aaaaaaaaaaaaaaaaaaaaaaaaV               0              25
  strStr   aaaaaaaaaaaaTaaaaaaaaaaaa               0              26
  strStr            good-for-nothing               0              33
  strStr                the birth of               0              39
  strStr           and the Lord said               0              51
  strStr   Raaaaaaaaaaaaaaaaaaaaaaaa               0              72
  strStr                 And the God               0            1024
  strStr                     ABCDABD           50569            3119
  strStr               CDAB ABCDABCD           50569            3953
--------------------------------------------------------------------
  strstr           and the Lord said               0              49
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaa               0              49
  strstr                the birth of               0              50
  strstr   aaaaaaaaaaaaaaaaaaaaaaaaV               0              50
  strstr   aaaaaaaaaaaaTaaaaaaaaaaaa               0              50
  strstr                 And the God               0              51
  strstr   Raaaaaaaaaaaaaaaaaaaaaaaa               0              52
  strstr            good-for-nothing               0              53
  strstr               CDAB ABCDABCD           50569             812
  strstr                     ABCDABD           50569            1033
--------------------------------------------------------------------
```


测试说明：

1.测试用例覆盖不够全面.

2.查询到一个词后只偏移一个内存位置.

3.时间包含了预处理时间。


具体测试逻辑见`main.c`

参考资料见具体算法实现注释.

