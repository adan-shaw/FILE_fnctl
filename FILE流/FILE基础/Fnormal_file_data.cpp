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
//g++ -o x ./Fnormal_file_data.cpp -g3


//file 存放应用数据测试解析:
/*
1.
	应用数据结构体 - 一个文件只能存放一个结构体?
	其实只要你按照sizeof(struct X)的大小来并排存放,
	也可以存放'多个结构体'的应用数据!!


	但一般只存放‘同一种类型的结构体’,
	因为如果存放多个不同类型的结构体,
	数据就容易发生错乱, 一旦取出长度不对,
	就会导致结构体提取后, 数据对齐出错,
	结果导致数据严重错乱!!
	(也就是: 数据边界不清晰了, 读写操作容易出问题)


	一般情况下, 用file 来存放的应用数据, 都是比较重要但不复杂的应用数据.
	否则难以hold 得住.
	虽然在单个结构体的情况下, 应用数据边界是清晰的,
	你可以在结构体内存放int,double,float,void*,char,char*等数据类型,
	而且数据边界清晰, 反复读写不会出现问题.

	但一般不会存放多个应用数据结构体,
	否则你要记住存放的结构体类型, 长度, 顺序, 从而确定读取的顺序和长度.
	但是写入的时候, 可能要面临重写.


2.
	file 不能存放的数据:
		*(一般不会存放类实体的, 一般可以存放结构体.)
		*指针buf(实体buf 就可以存放)
		*so 函数解析库, 类函数解析api方法(这也是为什么不存放类的原因)

	但是如果类函数的调用, 实际是一根函数指针,
	去调用elf封装下的函数共享实体,
	那么其实类还是可以存放的, 这里必须再造一个测试, 存放一个类.


	但是file 数据更新, 不能局部更新, 只能重写全部数据.
	如果需要局部更新, 可以使用mmap(), 必须使用mmap().



3.系统兼容性更新:
	新版的linux 操作系统, man 3 fopen() 中,
	已经剔除了对t text模式编辑, b 二进制模式编辑了.
	只剩下: r w a + 四种自由组合.
	也就是:
		新版linux 系统已经不区分文件类型了, 你直接写入应用数据,
		它就是应用数据存放文件, 你写入文本数据, 它就是文本数据文件.

	所以, t, b 选项或者是用来兼容旧系统的.



4.
	应用数据一般情况下是不允许打开的,
	跟文本数据不一样, 应用数据存放在文本里面,
	一般被破坏, 修复的可能性很低.
	如果你保持文本数据, 那么可以人工恢复,改一下文本就行.
	所以配置数据为什么要做成文本, 存放到文件里面, 是有一定道理的.

	应用数据应该是文本+二进制的集合.
	因为你存放字符串的时候, 你会看得到字符串, 但是变量倒变成了乱码.
	这跟二进制有点不一样

	应用数据在单系统下是安全的
	但是如果要将应用数据跨平台跨物理机器传递,
	应当将应用数据文本序列号, json,XML,就是文本序列化工具库



5.
	机器内部, 是可以通过很多种ipc 方式传递'类实体'的,
	但是有一个问题比较重要: 如果传递'类实体', 范围更小, 只能在同一进程之间.
	否则该进程就找不到'进程堆', 很多数据不能共享, 还有共享链接库.
	不知道类存放在shm共享内存中, 是否可以进行进程间传递.
	但是进程间传递'类实体'的意义何在？

	当然, 无论有数据边界的msg,udp socket or 无数据边界的管道, tcp socket,
	都可以传递类实体, 因为其实'类函数'只是一根函数指针而已.
	只要两个进程装载的.so 共享内存库, 是共享的即可.
	如果是定义了'静态编译', 这种类实体传递是失败的!!

	所以超级不建议你这样传递数据!!.



6.这个readv() 读取, 其实很适合读取应用数据!!
	相信writev() 也很适合写入应用数据.
	因为struct iovec iov[3];有多少个数据写入,每个数据多长,都需要验证好.
	这方面的需求, 与'应用数据file'写入非常符合.


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
