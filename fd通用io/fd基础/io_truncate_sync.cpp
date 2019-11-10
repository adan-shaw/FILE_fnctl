//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:
//	2019-05-13: 新增'tty 文本标准格式'风格
//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//本案例: 通用io的基础范本


//编译:
//g++ -o x ./io_basic.cpp -g3

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h> // for 原子操作O_RDONLY 之类的
#include <unistd.h>



//*************************
//带权限操作的fio 文件打开方式
//*************************
/*
	O_RDONLY = 只读
	O_WRONLY = 只写
	O_RDWR = 读写独占
	O_APPEND = 追加
	O_TRUNC = 截断模式-若文件存在,则长度被截为0,属性不变(即清空文件内容)
	O_CREAT = 不存在则创建

	fopen() mode 与 open() flags 关系对照表
				r					O_RDONLY
				w					O_WRONLY | O_CREAT | O_TRUNC
				a					O_WRONLY | O_CREAT | O_APPEND
				r+				O_RDWR
				w+				O_RDWR | O_CREAT | O_TRUNC
				a+				O_RDWR | O_CREAT | O_APPEND

	0777 = 最高权限打开该文件
	0666 = 读写权限
	0555 = 读, 执行
	0444 = 只读取
	0222 = 只写

	在POSIX 系统, 包含Linux下都会忽略b 字符.
	(也就是遵循posix的系统,都不允许b 二进制读取文件,只允许t 文本式读取文件)

	由fopen()所建立的新文件会具有:
		S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH(0666)权限
	此文件权限也会参考umask 值


	fdopen()mode 字符串则代表着文件指针的流形态,
	此形态必须和原先open()文件时, 描述词的'读写模式'相同.
*/

//测试函数
/*
	普通file 直写型io(但实际也不是真的立刻直写,还要看内核调度,只是尽快直写)
	内核对write()/read() 仍然有一定的缓冲调度机制, 但是这个机制是不可控的.
	fio 是程序员可控的io流机制.
	(可以避免频繁调用write()/read() api, 这两个api 速度很慢!!
		相比fio 的api, write/read 实在太慢了, 可能涉及一些磁盘信息获取操作.)
*/
void io_basic(void){
	int fd,tmp;
	char buf[32];



	//1.open() 打开文件-直写无缓冲
	fd = open("./test_data", \
			O_RDWR | O_CREAT | O_APPEND, 0600);
	if(fd == -1){
		perror("open() failed");
		return ;
	}


	//2.write() 直写文件
	//(socket 不支持游标pos lseek(), 所以只能原始的'块数据直写直读')
	//设备都需要这种块读写, pos 游标只是磁盘设备的专属
	strncpy(buf,"adan shaw",32);
	tmp = write(fd,buf,strlen(buf)+1);
	printf("write() len = %d size\n",tmp);//写入数据的长度


	//3.read() 直读文件
	lseek(fd,10,SEEK_SET);//以追加文件打开,不重置到文件头,只能读取到EOF
	memset(buf,'\0',32);
	tmp = read(fd,buf,31);
	//tmp = read(fd,buf,32);记住要保留多一字节,用来存放'\0',否则乱码
	printf("read() len=%d, %s size\n",tmp,buf);

	//4.lseek() -- 单位是字节!!
	lseek(fd,10,SEEK_CUR);//文件游标的当前pos, 向后偏移10 字节
	lseek(fd,10,SEEK_SET);//文件游标重置到文件头, 向后偏移10 字节
	lseek(fd,10,SEEK_END);//文件游标重置到文件尾, 向前偏移10 字节


	//5.lseek() + io 的原子操作(比较好用)
	//写
	tmp = pwrite(fd,"1234567890",11,SEEK_SET);
	printf("pwrite() len=%d size\n",tmp);
	//读
	memset(buf,'\0',32);
	tmp = pread(fd,buf,31,(SEEK_SET+10));//从SEEK_SET后移10字节,再读取31字节
	printf("pread()=%s, sum=%d size\n",buf);


	//6.立即截断文件(超出部分清除)
	//	注意, 不能对socket fd 进行截断, 有明文规定
	tmp = ftruncate(fd,32);//如果是fd 截断, fd 已经打开, 肯定成功.

	//不一定成功, 权限, 存在不存在等问题, 需要加if 判断
	tmp = truncate("./test_data",32);
	if(fd == -1){
		perror("truncate() failed");
		return ;
	}


	//7.立即执行磁盘同步(这个才是真正的直写)
	tmp = fsync(fd);//立刻同步fd 所有数据到磁盘
	tmp = fdatasync(fd);//同步fd 差异数据到磁盘
	tmp = sync();//立刻刷新内核所有的缓冲块
	if(fd == -1){
		perror("sync() failed");
		return ;
	}


	//8.close()
	//如果是常年运行的服务器程序, 这里close() 可能也要if 返回结果,
	//因为如果常年运行的服务器程序, close() 失败会造成程序fd 缺失的问题,
	//还会导致系统维护fd 的数据结构内存, 而导致内存溢出.

	//(但是一般分配无问题, 回收也没问题, close() 跟time() 一样稳定.)
	//主要是创建open()的时候, 需要if 一下, 不要这么胆小,
	//再说, 真的出问题, 你也可以调试出来, 不要乱加乱添
	close(fd);

	return;
}





int main(void){
	//普通file 直写型io(但实际也不是真的立刻直写,还要看内核调度,只是尽快直写)
	io_basic();

	return 0;
}

