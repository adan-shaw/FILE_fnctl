//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./read_write.cpp -g3


/*
1.read()是一个底层api,其他read系列函数,最终也会调用该函数(最底层的读操作)
ssize_t pread(int fildes, void *buf, size_t nbyte, off_t offset);
ssize_t read(int fildes, void *buf, size_t nbyte);
ssize_t readv(int fildes, const struct iovec *iov, int iovcnt);

例子:
char data[100];
n = read(STDIN_FILENO, data, 100);


read 函数从 filedes 指定的已打开文件中读取 nbytes 字节到 buf 中.
以下几种情况会导致读取到的字节数小于 nbytes ：
  A.读取普通文件时, 读到文件末尾还不够 nbytes 字节.
    例如：如果文件只有 30 字节, 而我们想读取 100 字节,
    那么实际读到的只有 30 字节, read 函数返回 30 .
    此时再使用 read 函数作用于这个文件会导致 read 返回 0 .
    (表明: read 读的时候, 文件光标也是在移动的.)
  B.从终端设备(terminal device)读取时, 一般情况下每次只能读取一行.
  C.从网络读取时, 网络缓存可能导致读取的字节数小于 nbytes 字节.
  D.读取 pipe 或者 FIFO 时,
    pipe 或 FIFO 里的字节数可能小于 nbytes .
  E.从面向记录(record-oriented)的设备读取时,
    某些面向记录的设备(如磁带)每次最多只能返回一个记录.
  F.在读取了部分数据时被信号中断.


注意：read不会自动加上'\0';
     对于'\n', 它也是原封不动的读入, 当做普通字符.



2.write()是一个底层api,其他write系列函数,最终也会调用该函数(最底层的写操作)
ssize_t pwrite(int fildes, const void *buf, size_t nbyte, off_t offset);
ssize_t write(int fildes, const void *buf, size_t nbyte);
ssize_t writev(int fildes, const struct iovec *iov, int iovcnt);



3.pos游标移动与读写结合的原子操作:
	lseek() + io 的原子操作(比较好用)
	//写
		tmp = pwrite(fd,"1234567890",11,SEEK_SET);
		printf("pwrite() len=%d size\n",tmp);//打印结果
	//读
		memset(buf,'\0',32);
		//(从SEEK_SET后移10字节,再读取31字节)
		tmp = pread(fd,buf,31,(SEEK_SET+10));
		printf("pread()=%s, sum=%d size\n",buf);//打印结果



4.io读写阻塞问题: read() or write() 系列函数, 操作可能会触发阻塞.



5.设备io读写, 与磁盘io读写的区别:
	write() 直写文件:
	(socket 不支持游标pos lseek(), 所以只能原始的'块数据直写直读')
	设备都需要这种'块读写', pos游标只是'磁盘设备'的专属.



6.这个readv() 读取, 其实很适合读取应用数据!!
	相信writev() 也很适合写入应用数据.
	因为struct iovec iov[3];有多少个数据写入,每个数据多长,都需要验证好.
	这方面的需求, 与'应用数据file'写入非常符合.

*/



#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <stdlib.h>



#define STR_SIZE 256
const char* test_file_path = "./test_data";



//多重缓冲区-写入或读取(简单read()/write()省略,不做实验)
int main(void){
	int fd;
	ssize_t numRead, totRequired;
	struct iovec iov[3];//多缓冲区容器(读写3个缓冲区,n=3)

	struct stat myStruct;	//第一个读入iov[0]的用户数据
	int x;								//第二个读入iov[1]的int数据
	char str[STR_SIZE];		//第三个读入iov[2]的字符串数据



	//打开文件(只读模式)
	fd = open(test_file_path, O_RDONLY);
	if(fd == -1){
		perror("open() failed");
		exit(EXIT_FAILURE);
	}


	//组装多缓冲区容器
	totRequired = 0;

	iov[0].iov_base = &myStruct;//填入第一个缓冲区
	iov[0].iov_len = sizeof(struct stat);
	totRequired += iov[0].iov_len;

	iov[1].iov_base = &x;//填入第二个缓冲区
	iov[1].iov_len = sizeof(x);
	totRequired += iov[1].iov_len;

	iov[2].iov_base = str;//填入第三个缓冲区
	iov[2].iov_len = STR_SIZE;
	totRequired += iov[2].iov_len;


	//执行多缓冲区-读取操作
	numRead = readv(fd, iov, 3);
	if(numRead == -1){
		perror("readv");
		exit(EXIT_FAILURE);
	}

	//测试读取的量是否溢出!!
	//numRead=readv()读取总量累计
	//totRequired=缓冲区最大容纳量.
	if(numRead < totRequired)
		printf("Read fewer bytes than requested\n");

	//打印numRead=readv()读取总量累计 | totRequired=缓冲区最大容纳量.
	printf("total bytes requested: %ld; bytes read: %ld\n",\
			(long) totRequired, (long) numRead);

	close(fd);



	exit(EXIT_SUCCESS);
}
