# 中国象棋通用引擎协议

英文: Universal Chinese Chess Interface(UCCI)

网址: <https://www.xqbase.com/protocol/cchess_ucci.htm>

作者: 象棋百科全书网 <webmaster@xqbase.com>

版本: 3.0.0-2007.11 (2007年11月修订版)

## **一、概述**

中国象棋通用引擎协议(Universal Chinese Chess
Interface ，简称UCCI)，是一种象棋界面和象棋引擎之间的基于文本的通讯协议。设立中国象棋通用引擎协议的目的有：

(1)
使一个“可视化象棋软件”可以使用不同的“核心智能部件”，这些核心智能部件称为“引擎”，凡是遵循UCCI的引擎，都可以被该可视化象棋软件(也称为“界面”)所调用；

(2) 针对所有遵循UCCI的引擎，都可以开发不同的界面，使其具有不同的功能。

这样，“可视化象棋软件”和“核心智能部件”实现了分离，使得一部分程序设计师能专注于前者(界面)的开发，而另一部分程序设计师能专注于后者(引擎)的开发，让中国象棋软件的设计工作系统化、分工化，提高软件设计效率。

UCCI是模仿国际象棋的UCI来制定的。UCCI是开放式的协议，并且具有UCI的所有特点，具体反映在象棋百科全书网所收录的以下文章中：

(1) [国际象棋引擎：穿越困惑(转载自《国际象棋译文苑》)](https://www.xqbase.com/protocol/protocol1.htm)
(2) [国际象棋引擎协议历史(转载自《国际象棋译文苑》)](https://www.xqbase.com/protocol/protocol2.htm)
(3) [国际象棋通用引擎协议](https://www.xqbase.com/protocol/uci.htm)

UCCI自诞生以来不断在发展和更新，但保持了对早期版本的兼容。

3.0版较2.3版改进的内容有：

  * 建议取消option反馈中的repetition和drawmoves选项，将selectivity选项改成randomness，增加promotion选项。

2.3版较2.2版改进的内容有：

  * 建议采用“毫秒”作为唯一的时间单位，参阅option反馈中的usemillisec选项。

以后UCCI还会不定期地更新，并继续保持对早期版本的兼容。UCCI界面和引擎设计者可访问以下资源，来获得最新的UCCI版本：

[http://www.xqbase.com/protocol/cchess_ucci.htm](http://www.xqbase.com/protocol/cchess_ucci.htm)

## **二、通讯方法**

不管是Windows还是UNIX平台，能被界面调用的引擎都必须是编译过的可执行文件，它跟界面之间通过“标准输入”和“标准输出”(即C/C++语言中的stdin和stdout)通道来通讯。如果引擎从Windows平台移植到UNIX平台，那么需要重新编译源代码(管道操作的程序也需要作适当修改)，或使用跨平台接口。

作为界面的设计，要启动一个引擎，Windows平台下可用CreateProcess()函数，UNIX平台下可用fork()和exec()函数，然后重定向到一个输入管道和一个输出管道，具体操作可参阅WinBoard/XBoard源程序的StartChildProcess()函数，或参阅中国象棋引擎ElephantEye源程序的<pipe.cpp>模块。

作为引擎的设计，通讯比界面略为简单(只需要对stdin和stdout操作)，只在检查stdin是否有输入时较为麻烦，具体操作可参阅Crafty源程序的<utility.c>模块的CheckInput()函数，或参阅中国象棋引擎ElephantEye源程序的<pipe.cpp>模块。

通常，界面向引擎发送的信息称为“指令”，而引擎向界面发送的信息称为“反馈”。在UCCI中，不管是指令还是反馈，都是以“行”为单位的，即每条指令和反馈都必须以“回车”(即C/C++语言中的'\n')结束。

注意：引擎用缓冲方式发出反馈(即C/C++语言中直接将字符串写入stdout)，那么每输出一行都必须用fflush()语句刷新缓冲区。

## **三、引擎的状态**

UCCI引擎在启动后，有三种状态。

(1) 引导状态。

引擎启动时，即进入引导状态。此时引擎只是等待和捕捉界面的输入，而界面必须用ucci指令让引擎进入接收其他UCCI指令的空闲状态(稍后会提到)。当然，引擎也可以保留使用其他协议的权利，例如引擎允许第一条有效指令是cxboard，这样引擎就转而进入CXBoard状态。

收到ucci只后，引擎要完成一系列初始化工作，以输出ucciok的反馈作为初始化结束的标志，进入空闲状态。如果引导状态下UCCI引擎收到其他指令，则可以退出。

(2) 空闲状态。

该状态下引擎没有思考(即几乎不占用CPU资源)，而只是等待和捕捉界面的输入(和引导状态类似)，接收这样几类指令：A.
设置引擎选项(setoption指令)，B. 设置引擎的内置局面(即让引擎思考的局面)及其禁止着法(position和banmoves指令)，C.
让引擎思考(go指令)，D. 退出(quit指令)。

(3) 思考状态。

引擎收到go指令后，即进入思考状态，以输出bestmove或nobestmove的反馈作为思考状态结束的标志(回到空闲状态)。该状态下引擎将满负荷运转(CPU资源占用率接近100%)，但仍旧需要捕捉界面的输入(只有在批处理模式下不会捕捉界面的输入)，接收两类指令：A.
中止思考(stop指令)，B. 改变思考方式(ponderhit指令)。

go指令只决定了引擎将按照什么思考方式来思考(即限定思考的深度，或限定思考的局面个数，或限定思考的时间)，而思考的局面则必须通过前面输入的position指令来告诉引擎。

其他注意事项有：

(1) 引擎只有在接收到go指令后才开始思考。即便引擎支持后台思考，在输出着法(反馈bestmove)后也不会自动进行，而是要由界面发送go
ponder指令，让引擎以后台思考方式进行思考。

(2)
bestmove的反馈并不改变引擎的内置局面，如果界面让引擎走棋，就必须读取bestmove反馈的着法，并在界面的局面上走完这一步(当然，界面也可以走别的着法)，再由position指令把新的局面告诉引擎。

(3)
如果对局是计时的，那么每次思考时都必须用go指令设定时钟，引擎仅仅根据时钟来决定分配多少时间来思考，回到空闲状态后时钟就失效了，必须由界面扣去引擎思考的时间(从发送go指令起到收到bestmove反馈结束)，在下次发送go指令时把新的时钟告诉引擎。

(4) 启用“批处理”模式时，引擎在思考状态下就不接收指令。批处理模式适合用重定向方式调试引擎，例如一个输入文件含有以下指令集：

```
    1: ucci
    2: setoption batch true
    3: position fen <fen_1>
    4: go depth 10
    5: position fen <fen_2>
    6: go depth 10
    7: quit
```

第4行以后引擎即进入思考状态，由于处于批处理模式，引擎反馈bestmove后回到空闲状态，才会继续接收以后的指令。如果没有第2行的启用批处理模式，那么第4行以后的指令都将在思考状态接收，而对于思考状态，这些指令都是无效的。

(5) 如果界面搞错了引擎的状态，在引擎的思考状态向界面发送quit指令，那么引擎最好能终止思考并立即退出，以避免界面无休止地等待引擎的退出。

(6) 如果界面搞错了引擎的状态，在引擎的空闲状态向引擎发送stop指令，那么引擎最好能反馈一个nobestmove，以避免界面无休止地等待引擎的反馈。

## 四、着法和棋盘的表示**

界面告诉引擎哪些着法是禁手(banmoves指令)，或者引擎回答界面应该走哪个着法(bestmove反馈)，这样的着法都用4个字符(简化的ICCS格式，参阅《[中国象棋电脑应用规范(二)：着法表示](cchess_move.htm)》一文)表示，即ICCS格式去掉中间的横线，并改成小写，例如h2e2。

界面用position指令把局面告诉引擎时，应该使用FEN串(写法参阅《[中国象棋电脑应用规范(三)：FEN文件格式](cchess_fen.htm)》一文)。但是对局中会遇到循环局面，引擎也必须考虑其对策，因此FEN串并不能完全反映局面信息，必须使用FEN串(当前局面前第一个不吃子的局面)和后续着法相结合的方法表示局面。例如，开局以后走了以下4步：

```
     1. 炮二平五 炮8平5 2. 炮五进四 士4进5

```

如果把这4步棋涉及的5个局面都告诉引擎，那么指令依次是：

```
    1: position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1
    2: position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves h2e2
    3: position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves h2e2 h7e7
    4: position fen rnbakabnr/9/1c2c4/p1p1C1p1p/9/9/P1P1P1P1P/1C7/9/RNBAKABNR b - - 0 2
    5: position fen rnbakabnr/9/1c2c4/p1p1C1p1p/9/9/P1P1P1P1P/1C7/9/RNBAKABNR b - - 0 2 moves d9e8
```

其中第4行更换了FEN串，因为该局面前一个着法是吃子着法。

## **五、指令和反馈**

按照惯例，指令用红色表示，反馈用蓝色表示。

**1.****ucci**

引导状态的指令。这是引擎启动后，界面需要给引擎发送的第一条指令，通知引擎现在使用的协议是UCCI。

**2.****id {name | copyright | author | user} <****信息****>**
引导状态的反馈。显示引擎的版本号、版权、作者和授权用户，例如：

    id name ElephantEye 1.6 Beta，说明引擎的版本号是ElephantEye 1.6 Beta；
    id copyright 2004-2006 www.xqbase.com，说明引擎的版权属于www.xqbase.com所有；
    id author Morning Yellow，说明引擎的作者是Morning Yellow；
    id user ElephantEye Test Team，说明引擎授权给用户ElephantEye Test Team使用。
**3.****option <****选项****> type <****类型****> [min <****最小值****>] [max
<****最大值****>] [var <****可选项****> [var <****可选项****> [...]]] [default
<****默认值****>]**

引导状态的反馈。显示引擎所支持的选项，<option>指选项的名称(后面会介绍)，选项的类型是label(标签，非选项)、button(指令)、check(是或非)、combo(多选项)、spin(整数)、string(字符串)中的一种。

通常的UCCI引擎支持以下选项：

(1)
usemillisec(check)，通知界面采用毫秒模式。建议引擎始终采用毫秒模式(即go指令的时间单位是“毫秒”)，并总是在ucciok前输出option
usemillisec ... 的反馈信息。除非引擎不发送 option usemillisec ...
的反馈信息，否则界面将自动使用毫秒模式，并向引擎发送 setoption usemillisec true
的指令。目前已知的UCCI界面程序(如象棋巫师、UCCI引擎联赛模拟器等)都采用这种做法；

(2) batch(check)，批处理模式(前面介绍过)，默认是关闭的；

(3) debug(check)，调试模式，默认是关闭的，打开后引擎会输出更多的信息(info反馈)，以帮助调试；

(4)
ponder(check)，是否使用后台思考的时间策略，默认是关闭的，设定该参数的目的仅仅是让引擎改变时间分配策略，而后台思考则仍然需要界面发出指令，参阅go
ponder和ponderhit指令；

(5) usebook(check)，是否使用开局库的着法，默认是启用的，如果关闭的话，即便当前局面在开局库中有着法，引擎也会不顾开局库而思考的；

(6) useegtb(check)，是否使用残局库，默认是启用的，和usebook类似；

(7)
bookfiles(string)，设定开局库文件的名称，可指定多个开局库文件，用分号“;”隔开，如不让引擎使用开局库，除了可以关闭usebook选项外，还可以把bookfiles设成空值；

(8) egtbpaths(string)，设定残局库路径的名称，和bookfiles类似；

(9)
evalapi(string)，设定局面评价API函数库文件的名称，和bookfiles类似，但只能是一个文件(例如，Windows下默认值是EVALUATE.DLL，Linux下默认值是libeval.so)；

(10) hashsize(spin)，以MB为单位规定Hash表的大小，0表示让引擎自动分配Hash表；

(11) threads(spin)，支持多处理器并行运算(SMP)的引擎可指定线程数(即最多可运行在多少处理器上)，0表示让引擎自动分配线程数；

(12)
idle(combo)，设定处理器的空闲状态，通常有none(满负荷)、small(高负荷)、medium(中负荷)、large(低符合)四种选项，引擎默认总是以满负荷状态运行的，而设置比较大的空闲状态，可以在人机对弈时留出适当的处理器资源，让用户运行其他程序；

(13) promotion(check)，是否允许仕(士)相(象)升变成兵(卒)，这是一种中国象棋的改良玩法，默认是不允许的(即默认采用常规走法)；

(14)
pruning(combo)，设定裁剪程度，裁剪越多则引擎的搜索速度越快，但搜索结果不准确的可能性越大，通常有none(无)、small(小)、medium(中)、large(大)四种，一般都设为large以充分展示引擎的搜索速度，但在处理一些刁难性的排局时，用large或medium不一定能解出，可尝试small或none；

(15)
knowledge(combo)，设定知识大小，通常知识量越多则程序的静态局面评价越准确，但的运算速度会变慢，该选项和pruning一样有四种设定，一般都使用large，但在解杀局时不需要静态局面评价，可以把知识量设置得小些；

(16)
randomness(combo)，设定随机性系数，和pruning一样有四种设定，一般都设为none，以保证引擎走出它认为最好的着法，但为了增强走棋的趣味性，可以把这个参数调高，允许引擎走出它认为不是最好的着法，以丰富走棋的样式；

(17) style(combo)，设定下棋的风格，通常有solid(保守)、normal(均衡)和risky(冒进)三种；

(18)
newgame(button)，设置新局或新的局面，引擎收到该指令时，可以执行导入开局库、清空Hash表等操作，UCCI界面《象棋巫师》在每次新建棋局或重新编辑局面时都会发送
setoption newgame 这条指令。

需要注意的是，各种引擎提供的选项内容是不一样的，也并不是所有的UCCI界面支持这些选项的，例如目前的UCCI界面《象棋巫师》没有对batch、debug等选项的设置(它们只供调试时使用)。

**4.****ucciok**

引导状态的反馈，此后引擎进入空闲状态。

**5.****isready**

空闲状态和思考状态的指令。检测引擎是否处于就绪状态，其反馈总是readyok，该指令仅仅用来检测引擎是否能够正常接收指令。

**6.****readyok**

空闲状态和思考状态的反馈。表明引擎处于就绪状态(可正常接收指令)。

**7.****setoption <****选项****> [<****值****>]**

空闲状态的指令。设置引擎参数，这些参数都应该是option反馈的参数，例如：

```
    setoption usebook false，不让引擎使用开局库；
    setoption selectivity large，把选择性设成最大；
    setoption style risky，指定冒进的走棋风格；
    setoption loadbook，初始化开局库。
```

但是，设置option反馈没有给出的参数，并不会出错。例如UCCI界面《象棋巫师》就从不识别option反馈，而直接根据用户的设置发送setoption指令。

**8.****position {fen <FEN****串****> | startpos} [moves <****后续着法列表****>]**
空闲状态的指令。设置“内置棋盘”的局面，用fen来指定FEN格式串，moves后面跟的是随后走过的着法，例如：

    position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves h2e2 h9g7
FEN格式串的写法参阅《[中国象棋电脑应用规范(三)：FEN文件格式](cchess_fen.htm)》一文。

moves选项是为了防止引擎着出长打着法而设的，UCCI界面传递局面时，通常fen选项为最后一个吃过子的局面(或开始局面)，然后moves选项列出该局面到当前局面的所有着法。

startpos表示开始局面，它等价于 fen
rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 。

**9.****banmoves <****禁止着法列表****>**

空闲状态的指令。为当前局面设置禁手，以解决引擎无法处理的长打问题。当出现长打局面时，棋手可以操控界面向引擎发出禁手指令。例如：

    position fen 1r2kab1r/2c1a4/n1c1b1n2/4p2N1/p1p6/1C4P2/P1P1P4/2N1B3C/4A4/1RBAK2R1 w - - 0 1 moves h6i4 i9h9 i4h6 h9i9
    banmoves h6i4
本例取自《[象棋竞赛规则](rule.htm)》(1999年版)棋例图三，由于大多数象棋引擎无法识别红方这种方式的长捉，所以在采用中国象棋协会的比赛规则时，遇到这种局面就必须给引擎发出禁手指令。下一次发送position指令后，前面设置过的禁止着法就取消了，需要重新设置禁止着法。

目前UCCI界面《象棋巫师》不识别长打禁手，所以不会向引擎发送banmoves指令。

**10.****go [ponder | draw] <****思考模式****>**
空闲状态的指令，此后引擎进入思考状态。让引擎根据position指令设定的棋盘来思考，各选项为思考方式，有三种模式可供选择：

(1) depth <深度> | infinite：限定搜索深度，infinite表示无限制思考(直到找到杀棋或用stop指令中止)。如果深度设定为0，那么引擎可以只列出当前局面静态评价的分数，并且反馈nobestmove。
(2) nodes <结点数>：限定搜索结点数。

(3) time <时间> [movestogo <剩余步数> | increment <每步加时>] [opptime <对方时间> [oppmovestogo <对方剩余步数> | oppincrement <对方每步加时>]]：限定时间，时间单位是秒(默认)或毫秒(启用毫秒制时)，movestogo适用于时段制，increment适用于加时制。opptime、oppmovestogo和oppincrement可以让界面把对方的用时情况告诉引擎。
如果指定ponder选项，则引擎思考时时钟不走，直到接受到ponderhit指令后才计时，该选项用于后台思考，它只对限定时间的思考模式有效。

指定draw选项表示向引擎提和，引擎以bestmove提供的选项作为反馈，参阅bestmove指令。

注意：ponder和draw选项不能同时使用，如果界面向正在后台思考中的引擎求和，则使用ponderhit draw指令。

**11.****info <****思考信息****>**

思考状态的反馈。显示引擎思考信息，通常有以下几种信息：

(1) time <已花费的时间> nodes
<已搜索的结点数>：思考信息中给出的时间通常以毫秒为单位，结点数和时间相除就是引擎的速度(NPS值)，单位是K。

(2) depth <当前搜索深度> [score <分值> pv
<主要变例>]：输出引擎思考到的深度及其思考路线和好坏。例如在起始局面下，《象棋巫师》收到引擎的反馈：info depth 6 score 4 pv
b0c2 b9c7 c3c4 h9i7 c2d4 h7e7，那么界面上应该输出：6 (+4) 马八进七 马２进３ 兵七进一 马８进９ 马七进六
炮８平５。分值通常以一个轻子(马或炮)为100分记，以上信息说明此时当前要走的一方占有相当于0.04个轻子的优势。

(3) currmove <当前搜索着法>：输出引擎正在思考的着法。

(4) message <提示信息>：输出引擎要直接告诉用户的信息，建议界面程序直接将提示信息显示在界面上。

**12.****ponderhit [draw]**

思考状态的指令。告诉引擎后台思考命中，现在转入正常思考模式(引擎继续处于思考状态，此时go指令设定的时限开始起作用)。

指定draw选项表示向引擎提和，引擎以bestmove提供的选项作为反馈，参阅bestmove指令。

**13.****stop**

思考状态的指令。中止引擎的思考。另外，后台思考没有命中时，就用该指令来中止思考，然后重新输入局面。

注意：发出该指令并不意味着引擎将立即回到空闲状态，而是要等到引擎反馈bestmove或nobestmove后才表示回到空闲状态，引擎应尽可能快地作出这样的反馈。

**14.****bestmove <****最佳着法****> [ponder <****后台思考的猜测着法****>] [draw | resign]**
思考状态的反馈，此后引擎返回空闲状态。显示思考结果，即引擎认为在当前局面下的最佳着法，以及猜测在这个着法后对手会有怎样的应对(即后台思考的猜测着法)。通常，最佳着法是思考路线(主要变例)中的第一个着法，而后台思考的猜测着法则是第二个着法。

在对手尚未落子时，可以根据该着法来设定局面，并作后台思考。当对手走出的着法和后台思考的猜测着法吻合时，称为“后台思考命中”。

draw选项表示引擎提和或者接受界面向引擎发送的提和请求，参阅go draw和ponderhit
draw指令。resign选项表示引擎认输。UCCI界面在人机对弈方式下，根据不同情况，可以对引擎的bestmove反馈中的draw和resign选项作出相应的处理：

(1) 如果用户提和，界面向引擎发出go draw或ponderhit draw指令，而引擎反馈带draw的bestmove，那么界面可终止对局并判议和；

(2) 如果用户没有提和，而引擎反馈带draw的bestmove，那么界面可向用户提和，用户接受提和则可终止对局并判议和；

(3) 如果引擎反馈带resign的bestmove，那么界面可终止对局并判引擎认输。

引擎应该根据当前局面的情况(由position指令给出)，以及界面是否发送了带draw的go或ponderhit指令，来考虑是否反馈带draw或resign的bestmove。

**15.****nobestmove**

思考状态的反馈，此后引擎返回空闲状态。显示思考结果，但引擎一步着法也没计算，表示当前局面是死局面，或者接收到诸如 go depth 0
等只让引擎给出静态局面评价的指令。

**16.****probe {fen <FEN****串****> | startpos} [moves <****后续着法列表****>]**
空闲状态和思考状态的指令。获取Hash表中指定局面的信息，引擎必须立刻在Hash表中查找该局面的状态，由pophash指令反馈。

该指令仅用于引擎的调试，设计者可以向引擎发送一系列probe指令，捕获到搜索树的大致信息。

**17.****pophash [bestmove <****最佳着法****>] [lowerbound <Beta****值****> depth
<****深度****>] [upperbound <Alpha****值****> depth <****深度****>]**

空闲状态和思考状态的反馈。输出由probe所指定的局面在Hash表中信息。

如果该局面没有记录在Hash表中，那么只反馈pophash即可。

**18.****quit**

空闲状态的指令。让引擎退出运转。

**19.****bye**

接收到quit指令后的反馈。引擎完成了退出运转前的准备工作，通知界面，引擎将在瞬间正常退出运转。界面收到该指令后，即可关闭输入输出通道。

## 六、用例

下面是一个后台思考的例子，描述了UCCI引擎中最难处理的部分。(从界面到引擎的)指令用红色表示，(从引擎到界面的)反馈用蓝色表示。

```
    ucci
    id name ElephantEye Demo
    option usemillisec type check default false
    option usebook type check default true
    ucciok
    setoption usemillisec true
    setoption usebook false
    position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1
    go time 300000 increment 0
    info depth 6 score 4 pv b0c2 b9c7 c3c4 h9i7 c2d4 h7e7
    info nodes 5000000 time 5000
    bestmove b0c2 ponder b9c7
```

在这个例子中，引擎执红，用户执黑，采用5分钟包干的时限。引擎启动后，界面即让引擎分析初始局面(不用开局库)，引擎给出最佳着法“马八进七”，然后是用户走子，引擎按照猜测着法“马２进３”作后台思考。此时，引擎消耗了5秒钟，还剩余295秒的时间。

    
    position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves b0c2 b9c7
    go ponder time 295000 increment 0
    info depth 6 score 4 pv c3c4 h9i7 c2d4 h7e7 h0g2 i9h9
此时用户走子了，下面分两种情况讨论。

(1) 如果用户走了引擎的猜测着法“马２进３”，那么后台思考命中：

    
    ponderhit
    info nodes 10000000 time 10000
    info depth 7 score 4 pv c3c4 h9i7 c2d4 h7e7 h0g2 i9h9 i0h0
    info nodes 15000000 time 15000
    bestmove c3c4 ponder h9i7
现在引擎走“兵七进一”，并且猜测对方会走“马８进９”。尽管这着棋引擎思考了15秒钟，但是前10秒钟用的是对手的时间，自己的时间是从ponderhit到bestmove的部分，所以自己的时间只消耗了5秒钟，因此还剩余290秒的时间。然后引擎继续后台思考。

    
    position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves b0c2 b9c7 c3c4 h9i7
    go ponder time 290000 increment 0
    ……
(2) 如果用户走了其他的着法(比如“卒３进１”)，那么后台思考没有命中，必须根据这个着法重新思考。

    stop
    info nodes 10000000 time 10000
    bestmove c3c4 ponder h9i7
    position fen rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1 moves b0c2 c6c5
    go time 295000 increment 0
    ……
如希望获得更详细的关于UCCI操作上的细节，可参考UCCI引擎ElephantEye的源程序及其说明材料，这里不再作过多的介绍。

**七、电脑象棋联赛**

电脑象棋联赛使用UCCI引擎，但是参赛引擎并不一定要支持UCCI的全部内容，只需要能跟“UCCI引擎联赛模拟器”正常通讯就可正常比赛了，模拟器相当于一个支持UCCI的界面。参赛引擎必须能够识别并正确处理的指令有：

(1) ucci；

(2) position fen ... [moves ...]；

(3) banmoves ...；

(4) go [draw] time ... increment ... [opptime ... oppincrement ...]；

(5) quit。

参赛引擎必须能够反馈的信息有：

(1) ucciok；

(2) bestmove ... [draw | resign]。
为了更好地让引擎适应模拟器，引擎最好能够实现以下功能：

(1) 支持毫秒制。即启动时有option usemillisec的反馈，能够识别并处理setoption usemillisec
true的指令。启用毫秒制以后，在时间非常紧缺的情况下，模拟器就会准确地把时间告诉引擎，否则只会粗略地给出秒数。

(2) 支持认输和提和，即当引擎觉得没有机会获胜时，可以用bestmove ... draw提和或接受提和，当引擎觉得没有能力抵抗时，可以用bestmove
... resign认输，节约比赛时间。

(3) 支持stop指令。当引擎超时后，模拟器会发送stop指令让引擎立即给出着法(立即反馈bestmove)，超过一定时间(如超过0.2秒)才判超时负。

另外，识别setoption指令不是必须的，但在联赛中也会有用。例如，如果引擎需要用setoption bookfiles
...来导入开局库，而模拟器不会自动向引擎发送这条指令，那么引擎必须建立配置文件，其中有setoption bookfiles
...这行指令，模拟器在启动引擎后，会把配置文件中的每行都作为指令发送给引擎的。

**八、和****UCI****的区别**

UCCI是从国际象棋通用引擎协议UCI移植过来的，沿用了大部分UCI的指令和反馈，但是为了适应中国象棋软件的需要，作了以下几点改动：

(1) 增加了banmoves指令，因为中国象棋有长打作负的规则。

(2) 把UCI的反馈option name <选项> type <类型> ...简化为option <选项> type <类型>
...。例如，UCI中有option name Hash Size type spin这条反馈，而在UCCI中则是option hashsize type
spin。这里去掉了name关键字，因为它总是跟在option后面，显得多此一举。另外，UCI允许选项由多个有大小写的单词组成(如Hash
Size)，这是因为UCI界面会直接把这个选项名称显示在对话框上。而UCCI中的选项只用一个全部由小写字母组成的单词，因为中国象棋的UCCI界面使用中文，因此界面上不能输出Hash
Size，而用“置换表”或其他可以看得懂的中文术语。这样，界面还不如识别一个更简单的hashsize(一个全部由小写字母组成的单词)，再翻译成“置换表”。

(3) 把UCI的指令setoption name <选项> [value <值>]简化为setoption <选项>
[<值>]。由于UCCI的选项是一个单词，所以第二个空格后的内容肯定是选项的值，因此name和value就显得多此一举了。

(4) UCCI明确了引擎所处的三种状态，以及这三种状态下适用的指令和反馈，为界面和引擎的程序设计提供了清晰的思路。

(5) UCCI以go time <时间> opptime <时间>的形式把双方的时间信息传达给引擎，而不是UCI的go wtime <时间> btime
<时间>，这样可以简化引擎解析时间的操作，如果引擎在指定用时策略时不考虑对方的用时，那么可以不理会opptime <时间>。

(6) UCCI明确了4种思考模式，使得指令的解析简单化了。

(7) UCCI规定position fen ... [moves
...]指令中的FEN串是当前局面前第一个不吃子的局面，后面再跟该局面的后续着法。而UCI则是用position startpos moves
...指令，把棋局从头到尾的着法全都列出来，会增加通讯通道的压力。
