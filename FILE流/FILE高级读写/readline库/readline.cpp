//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:
//	2019-05-13: 新增'tty 文本标准格式'风格
//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,



//编译:
//g++ -o x ./readline.cpp -g3 -lreadline

//介绍readline 库:
/*
1.readline库并不是c/c++ 标准库, 只是GNU 的一个project.
	需要安装才能用:
		apt-get install libreadline7 libreadline-dev
	libreadline7(只是运行库)
	libreadline-dev(这个才是编码库, 没有安装就找不到头文件!!)

	编译选项: -lreadline
	readline 是.so 库共享的, 需要添加编译选项才能用.



2.readline库的用法:
	2.1: 在程序运行过程中, 调用readline()函数, 读取stdin命令行输入命令!!

	2.2: 调用add_history()函数记录命令行执行过的命令.
			 这可以是:
			 	 启动时传入的args[], 但是记录这个有点麻烦,
			 	 再说tty 一般也会自动记录, 你也不需要做.
			 另一个是:
			 	 调用readline()函数时, 你可以将读到的命令,
			 	 直接add_history()记录到'.bash_history'文件中.

	2.3: Tab键补全命令(这个比较难做)
			 实现原理其实很简单: 就是非消耗性提取tty 的命令, 然后分析命令组成部分.
			 一般情况下, scanf(),readline()等函数, 都是消耗性的.
			 你先用readline相关的函数:
typedef char *rl_compentry_func_t(const char *text, int state);
char **rl_completion_matches(const char *text, rl_compentry_func_t *entry_func);
			 提取命令, 然后分析有没有自动补全的命令, 有就补全打印到tty命令行.
			 这个需要强大的'tty命令行窗口'的操作能力, 而且readline帮你实现了部分功能,
			 你可以快速实现这个功能.
			 (pass, 这个功能不做.)



3.关于纪录命令的好坏处:
	这个'历史命令记录'有两个层面:
		1.tty 当前自动记录, 无论你执行什么命令, 当前tty 都会自动记录.
			但是如果你关掉当前tty 之后, 之前执行过的命令就可以不存在了.

		2.add_history 添加到history 永久性记录.
			这是一种方便, 也可能导致重复操作的麻烦,
			你一下手快, 执行两次的风险是有的.

		3.但是如果是daemon程序, 根本没有做这个东西的必要,
			因为daemon程序会自动利用tty进行命令记录,
			当然你关掉tty之后, tty记录的命令可能会丢失, 但是这已经不重要了.
			由于add_history()需要读全命令, 读全命令和参数.
			所以如果记录启动命令, 需要:
				将argc[n], args[n]里面全部的所有字符串全部组合在一起,
				才能组成一条完整命令, 记录到'.bash_history'文件中.

	记录命令的好处:
		Up上,Down下键就能查看历史记录命令,
		但是有些关键命令, 容易导致重复执行的风险!!
*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>



int main(void){
	char *p;
	int tmp;



	//简单将readline() 读到的字符串, 记录到'.bash_history'文件中.
	//ps: (可以不执行这个shell命令)
	while(true){
		p = readline("myshell:");
		if(strncmp(p,"quit",4) == 0)//前4个字符是"quit"则退出
			break;
		else{
			add_history(p);//否则加入历史列表, 并执行!!
			system(p);//你也可以选择执行这个shell命令
		}
	}

	exit(EXIT_SUCCESS);
}
