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
//g++ -o x ./Fnormal_file_binary.cpp -g3


/*
	二进制数据, 一般也是一个文件, 一个二进制数据块.
	如果存在多块二进制数据块的存放, 则也可以存放, 但'数据边界问题'就没办法保证了.
	(也就是你放进去容易, 读出去的时候, 没边界就会容易变成乱码!!)

	只有在存放同一种二进制数据的情况下, 才能维持'数据边界完整'.

	fread()是阻塞的,fwrite()也是阻塞的,都是'同步'操作.
*/



#include <iostream>
#include <stdio.h>
#include <string.h>



char buf[1024*1024*4];//4 mb 的缓冲区



//1.测试从文件中, 读取出应用数据'存放目标结构体'
void std_stream_read(void){
	int tmp;
	FILE *fp;




	fp = fopen("./x", "r");//读取自身作为'二进制数据读取测试'
	if(fp == NULL){
		perror("fopen() failed");
		return;
	}

	tmp = fread(&buf,sizeof(char),sizeof(buf),fp);
	if(tmp <=0){
		perror("fread() failed");
		return;
	}

	fclose(fp);
	return;
}



//2.测试将应用数据'存放目标结构体', 写入文件中
void std_stream_write(void){
	int tmp;
	FILE *fp;



	fp = fopen("./x2", "w");
	if(fp == NULL){
		perror("fopen() failed");
		return;
	}

	tmp = fwrite(&buf,sizeof(char),sizeof(buf),fp);
	if(tmp <= 0){
		perror("fwrite() failed");
		return;
	}

	fclose(fp);
	return;
}



int main(void){
	//1.测试从文件中, 读取出应用数据'存放目标结构体'
	std_stream_read();


	//2.测试将应用数据'存放目标结构体', 写入文件中
	std_stream_write();

	exit(EXIT_SUCCESS);
}
