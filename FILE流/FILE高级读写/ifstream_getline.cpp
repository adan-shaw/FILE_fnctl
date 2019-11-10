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
//g++ -o x ./ifstream_getline.cpp -g3



//c++ FILE流text数据-序列化的实现:
/*

	c++
	这个东西不好做呀!! put() 只能压进去一个char,,, write 方法才是全写的,
	这也可能出现问题, write 可能不会一行行序列化的写, 而是全部格式化写进去

	getline 可以指定缓冲区, 不指定缓冲区, 默认分配255 大小的缓冲区.
	超出缓冲区截断, 但是仍然会返回截断后读取到的字符串, 和移动光标.
	跟fgets() 差不多, 只是会报告一行是否读完.
	fgets() 不知道一行是否读完, fgets() 同样不会触发cpu 溢出寄存器.



题外引用:
	strlen()函数的语义(返回字符串的长度)
	该函数实际功能是: 从该字符串的第一个地址开始遍历, 直到遇到结束符NULL.
	返回的长度大小, 不包括NULL.

题外引用2:
	操作可能会触发阻塞.



1、逐词读取, 词之间用空格区分
	ifstream fin("data.txt");  //#include <fstream>
	string s;                  //#include <string>
	while( fin >> s ){
		cout << "Read from file: " << s << endl;
	}
	fin.close(); //最后别忘了关闭


2、行读取, 将行读入字符数组, 行之间用回车换行区分
	ifstream fin("data.txt");
	const int LINE_LENGTH = 100;
	char str[LINE_LENGTH];
	//如果是终端, 就是cin.getline(str, LINE_LENGTH)
	while( fin.getline(str,LINE_LENGTH)){
		//默认遇到\n即结束读取, 最多读取99个字符, 并在结尾自动加\0.
		cout << "Read from file: " << str << endl;
	}


3、逐行读取, 将行读入字符串, 行之间用回车换行区分
	ifstream fin("data.txt");
	string s;
	//如果是终端, 就是getline(cin, s)
	while(getline(fin,s)){
		//注意这里s必须是string, 而不能是字符数组. s的末尾会自动有\0.
		cout << "Read from file: " << s << endl;
	}


 */



#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;



//#define BUF_MAX 10//错误的测试, 让缓冲区溢出, 查看截断读取效果.
#define BUF_MAX 4096



//测试字符串一行未读完, getline() 和c++ 流可以收到通知.
//也可以清楚流错误.(或者你用c 获取流错误, 应该也能获取相关信息)
void test_str_line_overflow(void);



int main(void){
	//测试字符串一行未读完, getline() 和c++ 流可以收到通知.
	test_str_line_overflow();


	exit(EXIT_SUCCESS);
}



//测试字符串一行未读完, getline() 和c++ 流可以收到通知.
void test_str_line_overflow(void){
	const char *filePath = "./test_data";
	char buf[BUF_MAX];
	ifstream fin;



	fin.clear(ios::goodbit);//修改ifstream 为无错误标记

	fin.open(filePath,ios::in);//用ifstream流, 打开文件"./test_data"
	if(fin.fail()){
		perror("fin.open() failed");
		return;
	}


	printf("1.读取1行2行数据, 并打印\n");
	memset(buf,'\0',BUF_MAX);
	fin.getline(buf,BUF_MAX);//读取一行, 截断长度为BUF_MAX
	if(fin.fail()){//是否读取失败？？
		perror("fin.getline() failed");
		return;
	}
	else
		cout<<buf<<endl;//打印读取结果



	//典型的c++ ifstrea 流判断机制
	if(fin.bad())
		//io 出错(慎用, 这个只要io没有出错, 它就正确, 不管文件结束与否)
		std::cout << "I/O error while reading\n";
	else if(fin.eof())
		//pos游标已经到达EOF 文件尾(不管出错, 到达文件尾部就结束)
		std::cout << "End of file reached successfully\n";
	else if(fin.fail())
		//遇到非整数数据(读取错误,或者到达文件尾部,或者读取不够一行, 都会触发)
		std::cout << "Non-integer data encountered\n";



	printf("\n\n\n");
	printf("2.逐行读取, 全部读取打印!!\n");

	//回到流的开始位置(由于没有二次打开文件, 上次读取后, pos游标已经移动)
	fin.seekg(ios::beg);

	//执行重头再读
	while(!fin.fail()){
		memset(buf,'\0',BUF_MAX);
		fin.getline(buf,BUF_MAX);
		if(fin.fail()){
			perror("fin.getline() failed");
			//fin.clear(ios::goodbit);//修改ifstream为无错误标记,继续向下读取
																//这个修改会导致死循环的产生!!
																//永远没有错误, 那么while()就不会退出
		}
		cout<<buf<<endl;//打印读取结果
	}


	fin.close();//关闭ifstream 流

	return;
}
