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
//g++ -o x ./fread_fwrite.cpp -g3


//c 语言FILE流'二进制/应用数据/块数据'写入方式:
/*
	块数据写法, 很简单, 一般是按照buf 长度写入,
	或者按照buf 大小截断读出.
	获取是全部读取/写入.

	如果写入应用数据, 请自己保证序列化问题, 可能还会有大小序的问题.
	如果你要将'导出后的应用数据文件'转移到第二个平台上面,
	这种方式不再适用, 你应该考虑: json/xml 做序列化转移.

	应用数据FILE读写, 详情请看: ../FILE基础/Fnormal_file_data.cpp


	fread()是阻塞的,fwrite()也是阻塞的,都是'同步'操作.(操作可能会触发阻塞)
*/



#include <iostream>
#include <stdio.h>
#include <string.h>



//应用数据'存放目标结构体'
typedef struct linux_data_file_test{
	int a;
	int b;
	double c;
	float d;
	void* p;
	char f;
	char* g;
	char h[64];
}test_t;


//应用数据'类'
typedef class linux_data_file_test2{
public:
	linux_data_file_test2(){};
	~linux_data_file_test2(){};
	int a;
	int b;
	double c;
}test_t2;



//1.测试将应用数据'存放目标结构体', 写入文件中
void std_stream_write(void){
	int tmp;
	test_t x;
	test_t2 x2;
	FILE *fp;



	//初始化结构体
	x.a = 999;
	x.b = 555555;
	x.c = 123.123;
	x.d = 987.6987;
	x.p = &x.h;
	x.f = 'A';
	x.g = x.h;
	strncpy(x.h,"stderr fwrite() 测试\n",64);

	//初始化类
	x2.a = 888;
	x2.b = 555;
	x2.c = 777.444;


	//fp = fopen("./test_data", "w");//以默认模式-自动匹配,打开文件,写入应用数据
	fp = fopen("./test_data", "wb");//以binary模式打开, 写入应用数据
	//fp = fopen("./test_data", "wt");//以text 模式打开, 写入应用数据
	if(fp == NULL){
		perror("fopen() failed");
		return;
	}


	tmp = fwrite(&x,sizeof(char),sizeof(x),fp);
	if(tmp <= 0 || ferror(fp)){
		perror("fwrite() failed");
		goto std_stream_write_failed;
	}

	tmp = fwrite(&x2,sizeof(char),sizeof(x2),fp);
	if(tmp <= 0 || ferror(fp)){
		perror("fwrite() failed");
		goto std_stream_write_failed;
	}

	fflush(fp);//指定冲刷fp

std_stream_write_failed:
	fclose(fp);
	return;
}



//2.测试从文件中, 读取出应用数据'存放目标结构体'
void std_stream_read(void){
	int tmp;
	test_t x;
	test_t2 x2;
	FILE *fp;



	//fp = fopen("./test_data", "r");//以默认模式-自动匹配,打开文件,读取应用数据
	fp = fopen("./test_data", "rb");//以binary模式打开, 读取应用数据
	//fp = fopen("./test_data", "rt");//以text 模式打开, 读取应用数据
	if(fp == NULL){
		perror("fopen() failed");
		return;
	}


	tmp = fread(&x,sizeof(char),sizeof(x),fp);
	if(tmp <= 0 || ferror(fp)){
		perror("fread() failed");
		goto std_stream_read_failed;
	}
	printf("x.a = %d\n",x.a);
	printf("x.b = %d\n",x.b);
	printf("x.c = %lf\n",x.c);
	printf("x.d = %f\n",x.d);
	printf("x.p = %s\n",x.p);
	printf("x.f = %c\n",x.f);
	printf("x.g = %s\n",x.g);
	printf("x.h = %s\n",x.h);

	tmp = fread(&x2,sizeof(char),sizeof(x2),fp);
	if(tmp <= 0 || ferror(fp)){
		perror("fread() failed");
		goto std_stream_read_failed;
	}
	printf("x2.a = %d\n",x2.a);
	printf("x2.b = %d\n",x2.b);
	printf("x2.c = %lf\n",x2.c);


std_stream_read_failed:
	fclose(fp);
	return;
}



int main(void){
	//1.测试将应用数据'存放目标结构体', 写入文件中
	std_stream_write();

	//2.测试从文件中, 读取出应用数据'存放目标结构体'
	std_stream_read();

	exit(EXIT_SUCCESS);
}
