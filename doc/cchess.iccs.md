# 中国象棋着法表示规范

英文: Internet Chinese Chess Server(ICCS)

网址: <https://www.xqbase.com/protocol/cchess_move.htm>

作者: 象棋百科全书网 <webmaster@xqbase.com>

版本: 1.0.0-2006.2 (2006年2月修订版)

## 一、棋盘的标记

象棋的着法表示，简而言之就是某个棋子从什么位置走到什么位置。通常，表示方法可以分为"纵线方式"和"坐标方式"两种，现在作简要说明：

(1) 纵线方式，它是中国象棋常用的表示方法，即棋子从棋盘的哪条线走到哪条线。
中国象棋规定，对于红方来说的纵线从右到左依次用"一"到"九"表示，黑方则是"１"到"９"(如图一所示)，这种表示方式体现了古代中国象棋研究者的智慧。

(2) 坐标方式，它是国际象棋常用的表示方法。
中国象棋也可以用这种方法来表示，按照国际象棋的规则，
纵线从左到右(红方)依次为a b c d e f g h i，
横线从下到上(红方)依次为0 1 2 3 4 5 6 7 8 9(如图三所示)。


```
　１２３４５６７８９       a b c d e f g h      　　ａｂｃｄｅｆｇｈｉ

　车马象士将士象马车      .-.-.-.-.-.-.-.-.     ９　车马象士将士象马车　９
　ㅏ＋＋＋＋＋＋＋ㅓ   8  |-|-|-|-|-|-|-|-|  8  ８　ㅏ＋＋＋＋＋＋＋ㅓ　８
　ㅏ砲＋＋＋＋＋砲ㅓ   7  |-|-|-|-|-|-|-|-|  7  ７　ㅏ砲＋＋＋＋＋砲ㅓ　７
　卒＋卒＋卒＋卒＋卒   6  |-|-|-|-|-|-|-|-|  6  ６　卒＋卒＋卒＋卒＋卒　６
　ㅏㅗㅗㅗㅗㅗㅗㅗㅓ   5  |-|-|-|-|-|-|-|-|  5  ５　ㅏㅗㅗㅗㅗㅗㅗㅗㅓ　５
　ㅏㅜㅜㅜㅜㅜㅜㅜㅓ   4  |-|-|-|-|-|-|-|-|  4  ４　ㅏㅜㅜㅜㅜㅜㅜㅜㅓ　４
　兵＋兵＋兵＋兵＋兵   3  |-|-|-|-|-|-|-|-|  3  ３　兵＋兵＋兵＋兵＋兵　３
　ㅏ炮＋＋＋＋＋炮ㅓ   2  |-|-|-|-|-|-|-|-|  2  ２　ㅏ炮＋＋＋＋＋炮ㅓ　２
　ㅏ＋＋＋＋＋＋＋ㅓ   1  .-.-.-.-.-.-.-.-.  1  １　ㅏ＋＋＋＋＋＋＋ㅓ　１
　车马相仕帅仕相马车                            ０　车马相仕帅仕相马车　０
　　　　　　　　　　       a b c d e f g h

　九八七六五四三二一                            　　ａｂｃｄｅｆｇｈｉ

 　　　 图一                     图二                        图三
```

## 二、棋子的名称

为方便表示，中国象棋的棋子名称除了用汉字以外，还可以用字母，字母可从国际象棋中稍加改动得到，而数字是为了方便棋谱的输入(以便用在数字小键盘上)(见表一)：

| 红方 | 黑方 | 字母 | 相当于国际象棋中的棋子 | 数字|
| ---|---|---|---|---  |
|帅 | 将 | K | King(王) | 1  |
|仕 | 士 | A | Advisor(没有可比较的棋子) | 2  |
|相 | 象 | B/E[1] | Bishop(象) | 3  |
|马 | 马 | N/H[2] | Knight(马) | 4  |
|车 | 车 | R | Rook(车) | 5  |
|炮 | 炮 | C | Cannon(没有可比较的棋子) | 6  |
|兵 | 卒 | P | Pawn(兵) | 7  |
表一 中国象棋棋子代号

[1] 世界象棋联合会推荐的字母代号为E(Elephant)
[2] 世界象棋联合会推荐的字母代号为H(Horse)


## 三、ICCS坐标格式

ICCS是中国象棋互联网服务器(Internet Chinese Chess Server)的缩写。在网络对弈服务器处理着法时，把着法表示成起点和终点的坐标是最方便的
，因此这种格式最早在计算机上使用。

```
　　１２３４５６７８９　　　　　　　　　１２３４５６７８９
　　ａｂｃｄｅｆｇｈｉ　            　　ａｂｃｄｅｆｇｈｉ

９　车马象士将士象马车　９          ９　车ㅜ象ㅜ将士象车＋　９
８　ㅏ＋＋＋＋＋＋＋ㅓ　８          ８　ㅏ＋＋＋士＋＋＋ㅓ　８
７　ㅏ砲＋＋＋＋＋砲ㅓ　７          ７　ㅏ砲马＋砲＋马＋ㅓ　７
６　卒＋卒＋卒＋卒＋卒　６          ６　卒＋卒＋＋＋卒＋卒　６
５　ㅏㅗㅗㅗㅗㅗㅗㅗㅓ　５   =>     ５　ㅏㅗㅗㅗㅗㅗㅗㅗㅓ　５
４　ㅏㅜㅜㅜㅜㅜㅜㅜㅓ　４          ４　ㅏㅜㅜㅜ炮ㅜㅜㅜㅓ　４
３　兵＋兵＋兵＋兵＋兵　３          ３　兵＋兵＋兵＋兵＋兵　３
２　ㅏ炮＋＋＋＋＋炮ㅓ　２          ２　ㅏ＋＋＋炮＋马＋ㅓ　２
１　ㅏ＋＋＋＋＋＋＋ㅓ　１          １　ㅏ＋＋＋＋＋＋＋ㅓ　１
０　车马相仕帅仕相马车　０          ０　车马相仕帅仕相＋车　０

　　ａｂｃｄｅｆｇｈｉ　　          　　ａｂｃｄｅｆｇｈｉ
　　九八七六五四三二一　　          　　九八七六五四三二一

 1. H2-E2 (炮二平五)   H7-E7 (炮８平５)
 2. E2-E6 (炮五进四)   D9-E8 (士４进５)
 3. H0-G2 (马二进三)   H9-G7 (马８进７)
 4. B2-E2 (炮八平五)   B9-C7 (马２进３)
 5. E6-E4 (前炮退二)   I9-H9 (车９平８)
 6. …… (如右图)
```

在[中国象棋通用引擎协议](cchess_ucci.htm)(UCCI协议)中，坐标格式得到进一步简化，例如 `H2-E2` 记作 `h2e2`，把符号限制在一个32位数据中，处理起来速度更快。

## 四、中文纵线格式

这种格式对于广大象棋爱好者来说再熟悉不过了，但是这里还是要说明两个重要的细节。

一、仕(士)和相(象)如果在同一纵线上，不用"前"和"后"区别，因为能退的一定在前，能进的一定在后。

二、兵要按情况讨论：

(1) 三个兵在一条纵线上：用“前”、“中”和“后”来区别；

(2) 三个以上兵在一条纵线上：最前面的兵用“一”代替“前”，以后依次是“二”、“三”、“四”和“五”；

(3)
在有两条纵线，每条纵线上都有一个以上的兵：
按照“先从右到左，再从前到后”(即先看最左边一列，从前到后依次标记为“一”和“二”，可能还有“三”，再看右边一列)的顺序，把这些兵的位置标依次标记为“一”、“二”、“三”、“四”和“五”，不在这两条纵线上的兵不参与标记。

如右下图局面，四个兵分别位于四线和六线，下表列举了几种走法的坐标格式和纵线格式。

```
中：１２３４５６７８９
　　ａｂｃｄｅｆｇｈｉ

９　＋＋＋＋将＋＋＋＋　９
８　ㅏ＋＋兵＋兵＋＋ㅓ　８
７　ㅏ＋＋＋兵＋＋＋ㅓ　７
６　ㅏ＋＋兵＋兵＋＋ㅓ　６
５　ㅏㅗㅗㅗㅗㅗㅗㅗㅓ　５
４　ㅏㅜㅜㅜㅜㅜㅜㅜㅓ　４
３　ㅏ＋＋＋＋＋＋＋ㅓ　３
２　ㅏ＋＋＋＋＋＋＋ㅓ　２
１　＋＋＋＋＋＋＋＋＋　１
０　＋＋＋＋帅＋＋＋＋　０

　　ａｂｃｄｅｆｇｈｉ
中：九八七六五四三二一
```


| 中文纵线格式 | 数字纵线格式 | 坐标格式|
|---|---|---  |
|一兵平五 | Pa.5 | F8-E8  |
|二兵平五 | Pb.5 | F6-E6  |
|兵五进一 | P5+1 | E7-E8  |
|三兵平五 | Pc.5 | D8-E8  |
|四兵平五 | Pd.5 | D6-E6  |


另外需要注意的是：

(1)
如果黑方出现数字，不管数字代表纵线标号还是前进或后退的格数，都用阿拉伯数字表示，在计算机中显示全角的数字。但是代表同一纵线上不同兵的“一二三四五”(它们类似于“前中后”的作用)例外，例如例局面红黑互换，那么某步着法就应该写成“一卒平５”。

(2)
在传统的象棋记谱中，如果发生以上这种情况，通常用五个字来表示，例如“前兵四平五”等，在计算机处理过程中就比较麻烦，因为4个汉字(一个汉字占16位)的着法可以储存在一个64位的字当中(在C语言中数据类型为__int64或long
long)，而增加到5个汉字就比较麻烦了。黑方用全角的数字是同一个道理。

## 五、WXF纵线格式

WXF是世界象棋联合会(World XiangQi
Federation)的缩写，在该组织建议的记谱方式中，汉字被字母和数字所代替，即“进”、“退”和“平”分别用符号“+”、“-”和“.”表示，“前”、“中”和“后”也分别用符号“+”、“-”和“.”表示，并且写在棋子的后面(例如“前炮退二”写成“C+-2”而不是“+C-2”)，多个兵位于一条纵线时，代替“前中后”的“一二三四五”分别用“abcde”表示(这种情况极少发生)。

另外，代表棋子名称的第一个字母，还可以用数字1到7表示，这是为了方便数字小键盘的输入，例如“炮二平五”可以记作“62.5“(6代表炮)选用符号“+”、“-”和“.”也是出于这个考虑。

## 六、棋子代号问题

符号纵线格式是世界象棋联合会(WXF)推荐的记谱格式，但是现在棋子的字母代号用法不一，主要问题集中在仕(士)、相(象)和马的翻译上。仕(士)译成Advisor、Bachelor、Guard等，相(象)译成Bishop、Elephant、Minister等，马译成Horse或Knight。

出于这点考虑，很多象棋软件都能识别不同的代号，例如ElephantBoard将B和E都解释为相(象)，H和N都解释为马，SaoLa则可以由用户自定义字母代号，等等。

**七、简体和繁体的识别问题**

无论采取哪种格式记谱，棋谱最终应该被象棋软件所识别。通常象棋软件都应支持以上三种格式，除“中文纵线格式”以外，其他两种格式都用英文和数字表示，不涉及中文字符，所以处理起来比较容易。而中文格式却非常麻烦，除了涉及到简体还是繁体外，还有字符集的问题。

象棋软件应该充分考虑这个问题，例如ElephantBoard在这个问题上就考虑得十分周到。在GB字符集(大陆地区)里，可识别简体和繁体字符，而在Big5字符集(港台地区)里，由字符集转换引起的问题也考虑进去了——表示棋子位置的“后”，由GB字符转换为Big5字符时，仍然写为“后”，但正确的写法是“後”，无论如何这两个“后(後)”都会被ElephantBoard认可。
