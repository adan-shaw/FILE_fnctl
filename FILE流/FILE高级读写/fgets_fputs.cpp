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
//g++ -o x ./fgets_fputs.cpp -g3

//c 语言FILE流text数据-序列化的实现:
/*
一、gets()
	char *gets(char *s);
	gets()是个不安全的函数, 应该坚决不用.
	因为:
		它每遇到换行符就会返回, 即使是一开始就遇到一个换行符.
		返回后会将换行符变为'\0'.
		由于gets()无法知道字符串的大小,
		遇到'\n 换行符'或'EOF文件结束符'才会结束,
		因此容易造成缓存溢出的安全性问题.

		C11标准已经不让用gets()了, 可以用fgets()来代替.
		比如对于同一个字符串str[50]：
		fgets()的用法是:
			fgets(str, 50, stdin);
			遇到'\n'换行符, EOF文件结束符, 或者缓冲区溢出,
			都会截断并结束fgets(), 返回读到的字符串.
			但fgets() 没法判断'\0'字符串结束符号,
			fgets() 是以'\n'作为结束标准的.



二、fgets()
	char *fgets(char *buf, int bufsize, FILE *stream);
	遇到'\n'换行符, EOF文件结束符, 或者缓冲区溢出,
	都会截断并结束fgets(), 返回读到的字符串.

	如果是'\n换行符'终结fgets(),
	则‘\n’会被保存在缓冲区中, 只是后面再加个'\0'.
	对于fgets()来说,'\n'是一个特别的字符,
	而'\0'并无任何特别之处, 读到'\0'就当作普通字符读入.


	如果文件中存在'\0'字符(或者0x00字节),
	fgets()就无法判断缓冲区中的'\0'究竟是‘从文件读上来的字符’,
	还是由fgets() 自动添加的结束符.

	所以fgets()只适合读'文本文件', 而不适合读'二进制文件',
	因为'文本文件'中的所有字符都应该是可见字符, 不能有'\0'.
	但是'二进制文件'并不理会'\0', '二进制文件'中没有特殊字符.

	补充：
		要解决问题, 就需要用到c++ 字符处理机制, 因为c++ 字符处理的确更强大.
		但基本上, text 类型读写, fgets() 已经不成问题了.
		但如果对应用数据进行操作, 用c++可能比较好.
		但它返回的是malloc()分配的内存, 需要用户自己free()掉缓冲区内存.
		详情请看: ifstream_getline.cpp

		c 语言也有类似的readline_getline.cpp 可供参考.

	补充2:
		主要是读取问题, 写入问题不大.
		而且fgets(), fputs() 已经足够应付'text 类型的文件数据'读写了.
		只是fgets(), fputs() 一次只能写一个字符串.
		如果是缓冲区块数据写入, 则失败.

		fgets(), fputs() 一般配合string 类实体写, 很安全.
		配合指针写, 缓冲区写, 必须保证只有一串字符串,
		否则可能会发生漏写



三、puts与fputs
	int puts(const char *str);
	int fputs(const char *str, FILE *fp);//注意区别于fputc(c, fin).

	fputs()向指定的文件写入一个字符串, puts()向'标准输出'写入一个字符串.
	两者都不会在结尾自动插入'\0',
	所以, 你写入的字符串必须有结束符号,否则程序可能会溢出.

	fputs()并不关心的字符串中的'\n'字符, 字符串中可以有'\n'也可以没有'\n'.
	puts()将字符串str写到标准输出, 然后自动写一个'\n'到标准输出.



四、阻塞问题:
	gets(),fgets(),puts(),fputs(),操作可能会触发阻塞.



五、
从函数原型上看, fgets()基本不会出错!!
char *fgets(char *s, int size, FILE *stream);
*/



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>



//1.fgets() test
void fgets_test(void);

//2.fputs() test
void fputs_test(void);



int main(void){
	printf("1.fgets() test\n");
	fgets_test();

	printf("\n\n\n2.fputs() test\n");
	fputs_test();


	exit(EXIT_SUCCESS);
}



//1.fgets() test
void fgets_test(void){
	FILE *fp;
	char buf[512];



	fp = fopen("./test_data", "rt");//以text 模式打开, 读取应用数据
	if(fp == NULL){
		perror("fopen() failed");
		return;
	}


	//测试ferror(fp)
	printf("ferror(fp)=%d\n\n",ferror(fp));


	//fgets()只能每次读取: 一个字符串,或一行字符!!没有其它用法！！
	//fgets()基本不会出错, 所以判断fgets() 调用正确的方法是:
	//if(fgets()==NULL || ferror(fp)), 文件结束or 流错误, 都是不行的
	memset(buf,'\0',512);
	if(fgets(buf,512,fp) == NULL || ferror(fp))
		printf("文件为空or 文件pos已经到达EOF终点, 需要重置. or 流发生错误\n");
	else
		printf("the first line: %s\n",buf);


	rewind(fp);//重置到SEEK_SET(文件头)

	//遍历逐行读取文件demo:
	printf("\nfgets() read all data from file(order by line)\n");
	while(!feof(fp) && !ferror(fp)){//未到达文件尾 && 没错流错误
		memset(buf,'\0',512);

		if(fgets(buf,512,fp) != NULL && !ferror(fp))
			printf("%s\n",buf);//读取正确, 打印读取到的字符串
		else//{
			//printf("ferror(fp)=%d\n\n",ferror(fp));//for test
			break;//文件结束or 流错误, 都退出while()
		//}
	}//FILE流出错,ferror(fp)返回非0,并退出while()


fgets_test_failed:
	fclose(fp);
	return;
}



//2.fputs() test
void fputs_test(void){
	FILE *fp;
	int tmp;
	const char* single_str = "hello world\n";
	char buf[64];//复杂字符串写入



	//fp = fopen("./test_data", "wt");//以text 模式打开, 重写模式
	fp = fopen("./test_data", "at");//以text 模式打开, 追加模式
	if(fp == NULL){
		perror("fopen() failed");
		return;
	}


	//简单字符串写入
	tmp = fputs(single_str,fp);
	if(tmp == EOF || ferror(fp)){//EOF == -1
		perror("fputs() failed");
		goto fputs_test_failed;
	}
	fflush(fp);//指定冲刷pfs


	//复杂字符串写入
	snprintf(buf,64,"%s%d%s\n","fuck",999,"you");//先格式化打印出复杂字符串

	tmp = fputs(buf,fp);//再将’复杂字符串buf‘压入FILE流
	if(tmp == EOF || ferror(fp)){
		perror("fputs() failed");
		goto fputs_test_failed;
	}
	fflush(fp);//指定冲刷pfs


	printf("fputs_test() success!!\n");//报喜

fputs_test_failed:
	fclose(fp);
	return;
}



