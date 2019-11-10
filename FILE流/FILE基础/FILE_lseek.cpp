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
//g++ -o x ./fio_basic.cpp -g3



//测试样例说明(各项fio 的基本操作)
/*
	fio 的光标移动-测试(普通FILE* 文件)
	fio_basic3();
*/


//注意:
//1.移动光标(注意所有fgets, fputs, fprintf, fwrite 之类的函数都会移动pos)
//2.\n 换行结束, fputs -- 不自动写入字符串结束标记符‘\0’
//	详细fgets,fputs 等函数涉及的东西, 请查看fgets_fputs_getline.h
//3.如果读到文件末尾返回EOF, EOF其实就是一个宏
//	#define EOF (-1)



#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // for 原子操作O_RDONLY 之类的
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>//for fchmod()



#define FS_BUF_LEN 4096



//创建fio 的主要方式:
//带权限操作的fio 文件关闭方式
inline void fcloseEx(FILE* retFs);
//带权限操作的fio 文件打开方式(先open()打开, 再转FILE* 的方式)
inline bool fopenEx(FILE** retFs, \
		const char* path, int auth, const char* mode);



//fio 的光标移动-测试(普通FILE* 文件)
void fio_basic3(void){
	FILE* pfs;
	int pos,tmp;
	fpos_t fpos;
	char fs_buf[FS_BUF_LEN];
	char buf[64];
	//char x1;



	//1.创建以0600 的方式, 打开一个文件, 并创建FILE*
	if(!fopenEx(&pfs,"./test_data",\
			O_RDWR | O_CREAT | O_APPEND,"a+")){
		printf("cpFILE_test()->fopenEx() fail\n");
		return;
	}


	//2.重新设置一个缓冲区
	setbuffer(pfs, fs_buf, FS_BUF_LEN-1);


	//3.显示当前的文件游标位置(long int 类型)
	pos = ftell(pfs);
	printf("now pos = %d\n",pos);


	//4.快速重置到SEEK_SET(文件头)
	rewind(pfs);
	pos = ftell(pfs);
	printf("now pos = %d\n",pos);


	//5.int fseek(FILE *stream, long offset, int fromwhere);
	//	pos将指向以fromwhere为基准, 偏移offset个字节的位置, 函数返回0
	//		SEEK_SET = 0
	//		SEEK_CUR = 1
	//		SEEK_END = 2
	fseek(pfs, 32, SEEK_CUR);//从当前位置开始,向后移动32 bit
	pos = ftell(pfs);
	printf("now pos = %d\n",pos);

	pos = fseek(pfs,0,SEEK_END);//从文件尾部开始,移动0bit(即直接移到文件尾)
	pos = ftell(pfs);
	printf("now pos = %d\n",pos);


	//6.流的读写-单个字符
	//fputc() 写一个字符到流中, 游标移动1
	rewind(pfs);//重置到SEEK_SET(文件头)
	fputc('x',pfs);
	//fgetc()从流读取一个字符, 移动游标1
	tmp = fgetc(pfs);
	if(tmp == EOF)
		printf("fgetc() failed: return = EOF = -1\n");
	else
		printf("fgetc(): %d\n",(char)tmp);


	//7.fgetpos() 获取file 在文件系统中的位置<文件的系统存储位置>
	//	(磁盘的起始头地址 or file system 的逻辑位置)
	if(fgetpos(pfs, &fpos) == -1){
		perror("fsetpos() failed");
		return;
	}
	else
		printf("fgetpos()= %d\n",fpos.__pos);


	//8.fsetpos() 移动file 在文件系统中的位置
	//	必须先fgetpos() 获取fpos_t 结构体
	fpos.__pos+=8;//移动8 bit
	if(fsetpos(pfs, &fpos) == -1){
		perror("fsetpos() failed");
		return;
	}


	//9.int fprintf (FILE *stream, const char *template, ...)
	//	格式化的打印数据到FILE *stream 中
	rewind(pfs);//重置到SEEK_SET(文件头)
	memset(buf,'\0',64);
	strncpy(buf,"i am adan shaw!!!!",64);
	fprintf(pfs,"<%s %d ::%s>\n",buf,1999,"llllove");
	fprintf(pfs,"***%s__%d***\n",buf,tmp=10);


	//10.int fscanf (FILE *stream, const char *template, ...)
	//	(这个函数没有截断, 不安全, 不要用.)
	//从FILE *stream 中格式化的读取数据
	rewind(pfs);
	memset(buf,'\0',64);
	fscanf(pfs, "%s%d%s", &buf, &tmp);
	if(tmp == EOF && errno != 0){
		perror("fscanf() failed");
		return;
	}
	else
		printf("buf1 = %s || a = %d\n", buf, tmp);


	//11.fwrite() 粒度控制-写(防止宽字符, 窄字符问题)
	//		第二个参数控制粒度,
	//		窄字符:
	//			char buf=""; sizeof(char); strlen(buf);
	//		宽字符:
	//			wchar_t buf=L""; sizeof(wchar_t); wcslen(buf);
	fseek(pfs,0,SEEK_END);//移动到文件尾部
	memset(buf,'\0',64);
	strncpy(buf,"i am adan shaw!!!!",64);
	fwrite(buf, sizeof(char), tmp=strlen(buf), pfs);


	//12.fread() 粒度控制-读(第二个参数控制粒度)
	memset(buf,'\0',64);
	rewind(pfs);
	fread(buf, sizeof(char), sizeof(buf)-1, pfs);
	printf("buf = %s\n", buf);


	//13.冲刷io 流
	fflush(pfs);//指定冲刷pfs
	fflush(NULL);//冲刷进程内所有的流, 全部冲掉, 包括stdin,stdout等


	fclose(pfs);
	return ;
}



int main(void){
	printf("1.fio 的光标移动-测试(普通FILE* 文件)\n");
	fio_basic3();

	exit(EXIT_SUCCESS);
}





//fdopen()mode 字符串则代表着文件指针的流形态,
//此形态必须和原先open()文件时, 描述词的'读写模式'相同.
inline bool fopenEx(FILE** retFs, \
		const char* path, int auth, const char* mode){
	//先解决文件权限问题
	int fd = open(path,auth,0600);//读写



	if(fd == -1){
		printf("fopenEx()->open() fail, errno = %d\n",errno);
		return false;
	}

	//修改文件的访问权限, 不让其他人访问(很有用!!)
	fchmod(fd,0600);

	//再创建fio 流-缓冲区
	*retFs = fdopen(fd, mode);
	fd = ferror(*retFs);
	if(fd != 0){
		printf("ferror() = %d\n",fd);
		return false;
	}
	return true;
}

//带权限操作的fio 文件关闭方式
inline void fcloseEx(FILE* retFs){
	//int fd = fileno(retFs);
	fclose(retFs);
	//close(fd);
}
