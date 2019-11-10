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
//g++ -o x ./scanf_printf.cpp -g3

//格式化输入输出系列'汇总':
/*

1、scanf()格式化输入系列
	#include <stdio.h>//文件流格式化输入
	int scanf(const char *format, ...);//tty格式化输入
	int fscanf(FILE *stream, const char *format, ...);//文件流格式化输入
	int sscanf(const char *str, const char *format, ...);


	ps: 实际上, ... 即’va_list ap‘ 参数列表,
			写法不一样, 但是都是表示多输入参数.

	#include <stdarg.h>//参数格式化输入
	int vscanf(const char *format, va_list ap);
	int vsscanf(const char *str, const char *format, va_list ap);
	int vfscanf(FILE *stream, const char *format, va_list ap);
	如果scanf()在开头遇到前次函数留下的空格或换行符,
	scanf()一律跳过,继续运行;


	如果在中间遇到空格或遇到换行符, scanf()将会停止.
	在读取的字符串后加上'\0'.




2、printf()格式化输出系列：
#include <stdio.h>//文件流格式化输出
int printf(const char *format, ...);
int fprintf(FILE *stream, const char *format, ...);

int dprintf(int fd, const char *format, ...);
int sprintf(char *str, const char *format, ...);
int snprintf(char *str, size_t size, const char *format, ...);


#include <stdarg.h>//参数格式化输出
int vprintf(const char *format, va_list ap);
int vfprintf(FILE *stream, const char *format, va_list ap);

int vdprintf(int fd, const char *format, va_list ap);
int vsprintf(char *str, const char *format, va_list ap);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);



*/


//3.常用的’格式化输出-输入‘for 内存缓冲区 or 文件流
/*
ps:	其实scanf()输入to内存缓冲区, or printf()输出to内存缓冲区,
		效果都是一样的, 都是从字符串缓冲区中读取数据,
		然后转移输入, 或者打印输出, 其实不变的都是:
			源数据, 是字符串数据, 或者变量数据!! from 变量"%s-%d-c"格式化.
			目的地, 缓冲区buf or 文件流.
		所以, 只要'源数据'与'目的地'的关系不变, scanf, printf的差别不大.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



//1.格式化-'缓冲区'输入to缓冲区/file文件流-字符串截断式(scanf()系列)
//这类函数十分难用！！
void myscanf(void);

//2.格式化-'缓冲区'输出to缓冲区/file文件流-字符串截断式(printf()系列)
void myprintf(void);



int main(void){
	//1.格式化-'缓冲区'输入to缓冲区/file文件流-字符串截断式(scanf()系列)
	//这类函数十分难用！！
	myscanf();

	//2.格式化-'缓冲区'输出to缓冲区/file文件流-字符串截断式(printf()系列)
	myprintf();


	exit(EXIT_SUCCESS);
}





//1.格式化-'缓冲区'输入to缓冲区/file文件流-字符串截断式(scanf() 系列)
//这类函数十分难用！！
void myscanf(void){
	char buf[64];
	int tmp;



	printf("1.1: myscanf() fscanf()流测试\n");
	errno = 0;
	tmp = fscanf(stdin,buf);
	if(errno != 0 && tmp <= 0){
		perror("fscanf() failed");
		//return;
	}
	else
		printf("%d=fscanf()\n",tmp);


	printf("1.2: myscanf() sscanf()buf测试\n");
	memset(buf,'\0',64);
	errno = 0;
	tmp = sscanf(buf,"%s%d%s","fuck",999,"you");
	if(errno != 0 && tmp <= 0){
		perror("sscanf() failed");
		return;
	}
	else
		printf("%d=sscanf()\n",tmp);

	printf("%s\n",buf);
	printf("足以证明:\n如果你想格式化输入到buf中,scanf()系列函数是不可用的\n");
	printf("因为scanf()系列函数, 没有一个版本是支持溢出截断的!!\n");

	return;
}



//2.格式化-'缓冲区'输出to缓冲区-字符串截断式(printf() 系列)
void myprintf(void){
	char buf[64];
	int tmp;
	int fd;



	printf("1.1: myprintf() fprintf()流测试\n");
	tmp = fprintf(stdout,"%s%d%s","fuck",999,"you");
	if(tmp == -1){
		perror("fprintf() failed");
		return;
	}

	printf("1.2: myprintf() dprintf()流测试(略)\n");
	/*
	tmp = dprintf(fd,"%s%d%s","fuck",999,"you");
	if(tmp == -1){
		perror("fprintf() failed");
		return;
	}
	*/

	printf("1.3: myprintf() snprintf()buf测试(优秀!!带截断功能!!)\n");
	memset(buf,'\0',64);
	tmp = snprintf(buf,64,"%s%d%s","fuck",999,"you");
	if(tmp == -1){
		perror("snprintf() failed");
		return;
	}
	printf("%s\n",buf);

	return;
}



/*
void unknow(void){
	std::string s;
	FILE *fin=fopen("./test_data","r");
	FILE *fout=fopen("./test_data","w");
	while(fscanf(fin,"%c",&c)!=EOF)
	{
		s+=c;
		fprintf(fout,"%s",s.c_str());
		fputc(c,fout);
	}
	fclose(fin);
	fclose(fout);
}
*/


