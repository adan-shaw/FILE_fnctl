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
//g++ -o x ./Fnormal_file_text.cpp -g3


//参照目录:
/*
C语言text 序列化: 参考../FILE高级读写/fgets_fputs.cpp
C语言text 序列化2: 参考../FILE高级读写/readline.cpp

C++语言text 序列化: 参考../FILE高级读写/ifstream_getline.cpp
										 ../FILE高级读写/ofstream.cpp

如果不进行序列化, 整个文件text 全部读取, 可以参考:fread_fwrite.cpp
(没啥研究意义)
 */



//1.
/*

	linux 用file 来存放text 数据, 一般可以用'line行'来划分.
	按'line行'来读写, 比较常见.(数据边界清晰)

	如果乱写, 那么就会面临: 无法确定字符串终结位置.
	那么, 你只能按照'已知位置'读取/写入(需要先定位),
	或者一次读取整个文件, 全部的text数据.
	(也就是数据边界不清晰)

	正常使用的情况下, 都是按'line行'来划分文件, 比较常见.
	read line or write line
	这就是linux file 操作最常见的一种.

	常用的FILE 流序列化函数: fgets() and fputs()
	详细解析, 请看:fgets_fputs_getline.h

*/



//2.关于为什么不用'通用io'做文本text 类型数据的序列化:
/*
	通用io 不能使用fgets() 系列函数, 是很难做到官方序列化支持的.
	所以一般文本text 序列化, 都不会用'通用io'来做, 要做还得自己实现.
	自己实现'通用io', 你就要定义一个'行MAX最大字符容量',
	超出这个'行MAX最大字符容量', 则自动换行, 写到下一行中.
	这个具体实现比较麻烦, 你有空可以试试.

	而且你要解决:
		直写磁盘的问题, 虽然系统仍然有磁盘读写缓冲机制,
		但是始终写的频率会频繁很多, 性能也不好.

	所以:
		'通用io'只做文本text全部提取读写测试!!
		不做序列化测试.
*/



#include <iostream>
#include <stdio.h>
#include <string.h>



const char* test1 = "i am the test1";
const char* test2 = "i am the test2";
const char* test3 = "i am the test3";
const char* test4 = "i am the test4";
const char* test5 = "i am the test5";



//1.按'line行'读写(有数据边界)
void std_stream_text_line(void);



//2.无数据边界, 这个文件读取, 或者按照已知位置, 定位定长读取.
void std_stream_text_all(void);






int main(void){
	//1.按'line行'读写(有数据边界)
	std_stream_text_line();

	//2.无数据边界, 这个文件读取, 或者按照已知位置, 定位定长读取.
	std_stream_text_all();


	exit(EXIT_SUCCESS);
}



//1.按'line行'读写(有数据边界)
void std_stream_text_line(void){

}



//2.无数据边界, 这个文件读取, 或者按照已知位置, 定位定长读取.
void std_stream_text_all(void){

}


