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



//简要声明:
//一: FILE* 流对象的基本属性(每个流对象保有下列信息):
//		FILE* 一般指文件流, 不过也可以是stdin stdout 等标准流
/*
	1) (C95)字符宽度: 未设置、窄或宽.
	2) 缓冲状态: 无缓冲、行缓冲、全缓冲.
	3) 缓冲区, 可为外部的用户提供缓冲区所替换.
	4) I/O 模式: 输入、输出或更新(兼具输入与输出).
	5) 二进制/文本模式指示器.
	6) 文件尾指示器.
	7) 错误状态指示器.
	8) 文件位置指示器(fpos_t 类型对象),
		 对于宽字符流包含剖析状态(mbstate_t 类型对象(C95)).
	9) (C11)在多个线程读、写、寻位或查询流时避免数据竞争的再入锁.
*/



//二: 测试样例说明(各项fio 的基本操作)
/*
	1.fio 基础操作1-测试(普通FILE* 文件)
	fio_basic1();

	2.fio 基础操作2-测试(临时FILE* 文件)
	fio_basic2();
*/




#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> // for 原子操作O_RDONLY 之类的
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>//for fchmod()



#define FS_BUF_LEN 4096



//三: 创建fio 的主要方式:
//带权限操作的fio 文件关闭方式
inline void fcloseEx(FILE* retFs);
//带权限操作的fio 文件打开方式(先open()打开, 再转FILE* 的方式)
inline bool fopenEx(FILE** retFs, \
		const char* path, int auth, const char* mode);





//*1.fio 基础操作1-测试(普通FILE* 文件)
void fio_basic1(void){
	FILE* pfs;
	int fd_test9,tmp;
	int fs_err;
	char fs_buf[FS_BUF_LEN];
	char buf_tmp[64];
	//char *fs_buf = (char*)malloc(FS_BUF_LEN);



	//1.创建以0600 的方式, 打开一个文件, 并创建FILE*
	if(!fopenEx(&pfs,"./test_data",\
			O_RDWR | O_CREAT | O_APPEND,"a+")){
		printf("cpFILE_test()->fopenEx() fail\n");
		return;
	}


	//2.重新设置一个缓冲区
	//	void setbuf(FILE *stream, char *buf);
	//	void setbuffer(FILE *stream, char *buf, size_t size);
	//	void setlinebuf(FILE *stream);
	//	以上其它三个调用, 在功能上, 只是setvbuf()调用的一个别名而已.
	//
	//int setvbuf(FILE *stream, char *buf, int mode, size_t size);
	//int mode:
	//	IONBF == 不缓存
	//	IOLBF == 行缓存
	//	IOFBF == 全缓存
	//*I.等价于setbuf()/setbuffer()
	//setvbuf(stream, buf, buf ? _IOFBF : _IONBF, BUFSIZ);
	//*II.等价于setlinebuf()
	//setvbuf(stream, (char *) NULL, _IOLBF, 0);
	tmp = setvbuf(pfs, fs_buf, fs_buf ? _IOFBF : _IONBF, FS_BUF_LEN-1);
	if(tmp != 0){
		printf("fio_basic1() setvbuf() fail, errno = %d\n",errno);
		fclose(pfs);
		return;
	}


	//3.获取文件流错误(before)//FILE流出错,ferror(fp)返回非0
	fs_err = ferror(pfs);
	if(ferror(pfs))
		printf("FILE流出错!!ferror(fp) != 0");

	printf("file stream error is: %d\n",fs_err);


	//4.fputs() 向FILE* 流中写入一串字符串(长度不限,遇到'\0'结束)
	tmp = fputs("welcome to new york！！",pfs);
	if(tmp == EOF || ferror(pfs)){
		perror("fputs() failed");
		fclose(pfs);
		return;
	}

	//5.截断读取一串字符串, 遇到'\0' 结束或, 缓冲区'溢出截断'结束
	if(fgets(buf_tmp,64,pfs) == NULL || ferror(pfs)){
		perror("fgets() failed");
		fclose(pfs);
		return;
	}


	//6.清除流错误(clear after)
	clearerr(pfs);


	//7.判断文件是否到了文件尾部(end-of-file)
	//	到了文件尾部, 返回true.
	if(feof(pfs))
		printf("pos游标已经移动到文件尾部'end-of-file'\n");
	else
		printf("pos游标还未移动到文件尾部'end-of-file'\n");


	//为test9 做准备
	fd_test9 = fileno(pfs);


	//8.关闭FILE* 文件流
	fclose(pfs);


	//9.检验FILE* 是否已经关闭的证据:
	//9.1:
	assert(close(fd_test9) == -1 && errno == EBADF);
	//9.2:
	assert(fcntl(fd_test9,F_GETFD) == -1 && errno == EBADF);
	fd_test9 = 0;//放空fd_test9


	return;
}





//*2.fio 基础操作2-测试(临时FILE* 文件)
void fio_basic2(void){
	FILE* pfs;
	int fd;
	int tmp;
	char fs_buf[FS_BUF_LEN];
	char tmpfile_proc_path[255] = {'\0'};//单个文件名max = 255
	char tmpfile_path[4096] = {'\0'};//文件路径长度max = 4096



	//1.创建一个临时文件
	pfs = tmpfile();
	if(pfs == NULL){
		printf("tmpfile() fail, errno = %d\n", errno);
		return ;
	}


	//2.重新设置一个缓冲区
	setbuffer(pfs, fs_buf, FS_BUF_LEN-1);


	//3.反向获取临时文件的文件名
	fd = fileno(pfs);

	//截断字符串打印
	snprintf(tmpfile_proc_path,\
			sizeof(tmpfile_proc_path), "/proc/self/fd/%d", fd);
	tmp = readlink(tmpfile_proc_path,tmpfile_path,sizeof(tmpfile_path)-1);
	if(tmp == -1){
		printf("readlink() fail, errno = %d\n", errno);
		return ;
	}
	else{
		printf("file_path:%s\n",tmpfile_path);
		printf("proc_path:%s\n",tmpfile_proc_path);
	}


	//关闭临时文件
	fclose(pfs);

	return ;
}









int main(void){
	printf("1.fio 基础操作1-测试(普通FILE* 文件)\n");
	fio_basic1();
	printf("\n\n");

	printf("2.fio 基础操作2-测试(临时FILE* 文件)\n");
	fio_basic2();
	printf("\n\n");

	exit(EXIT_SUCCESS);
}







//*************************
//带权限操作的fio 文件打开方式
//*************************
//O_RDONLY = 只读
//O_WRONLY = 只写
//O_RDWR = 读写独占
//O_APPEND = 追加
//O_TRUNC = 截断模式-若文件存在,则长度被截为0,属性不变(即清空文件内容)
//O_CREAT = 不存在则创建
//
//fopen() mode 与 open() flags 关系对照表
//			r					O_RDONLY
//			w					O_WRONLY | O_CREAT | O_TRUNC
//			a					O_WRONLY | O_CREAT | O_APPEND
//			r+				O_RDWR
//			w+				O_RDWR | O_CREAT | O_TRUNC
//			a+				O_RDWR | O_CREAT | O_APPEND
//
//0777 = 最高权限打开该文件
//0666 = 读写权限
//0555 = 读, 执行
//0444 = 只读取
//0222 = 只写
//
//在POSIX 系统, 包含Linux下都会忽略b 字符.
//(也就是遵循posix 的系统,都不允许b 二进制读取文件,只允许t 文本式读取文件)
//
//由fopen()所建立的新文件会具有:
//	S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH(0666)权限
//此文件权限也会参考umask 值


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







/*
//3.错误的FILE* 拷贝方式:
//注意, 这是一种错误的dup(FILE*) 方法!!
//FILE* 不需要拷贝, 而是可以直接再次创建FILE*, 打开同一个文件.
//**************************************************
tmp = fileno(pfs_test);//FILE* to int
tmp2 = dup(tmp);//先cp 一个fd 做备用
pfs_dup_err = fdopen(tmp2,"w+");//int to FILE*
if(pfs_dup == NULL){
	perror("fdopen() failed");
	fclose(pfs_test);
	fclose(pfs_dup);
	return;
}
//此处造成内存溢出, 证明这种拷贝方式是bullshit !!
//else
	//fclose(pfs_dup_err);//正确, 则释放该测试FILE*
//**************************************************
*/










