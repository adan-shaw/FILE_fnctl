//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -o x ./link_chmod.cpp -g3



#include <stdio.h>
#include <sys/stat.h>
#include <limits.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>




#define BUF_SIZE PATH_MAX
#define TEST_FIELNAME "./test_data"
#define TEST_LINK_NAME "./LINK_test_data"



int main(int argc, char *argv[]){
	struct stat statbuf;
	char buf[BUF_SIZE];
	ssize_t numBytes;



	//0.创建link
	if(symlink(TEST_FIELNAME, TEST_LINK_NAME) == -1){
		perror("symlink() failed");
		exit(EXIT_FAILURE);
	}


	//1.读取路径目标stat info
	if(lstat(TEST_LINK_NAME, &statbuf) == -1){
		perror("lstat() failed");
		exit(EXIT_FAILURE);
	}


	//2.判断是否是Link
	if(!S_ISLNK(statbuf.st_mode)){
		printf("%s is not a symbolic link", TEST_LINK_NAME);
		exit(EXIT_FAILURE);
	}


	//3.读取link的内容到string buf 中(并非link 指向的目标的内容)
	numBytes = readlink(TEST_LINK_NAME, buf, BUF_SIZE - 1);
	if(numBytes == -1){
		perror("readlink() failed");
		exit(EXIT_FAILURE);
	}
	buf[numBytes] = '\0';//Add terminating null byte
	printf("readlink: %s --> %s\n", TEST_LINK_NAME, buf);//打印读取到的link 内容


	//4.相对路径转换成绝对路径
	if(realpath(TEST_LINK_NAME, buf) == NULL){
		perror("realpath() failed");
		exit(EXIT_FAILURE);
	}
	printf("realpath: %s --> %s\n", TEST_LINK_NAME, buf);//打印绝对路径



	//2.
	//unlink() 也有删除功能, 如果是link 快捷方式, 则只会减少一个快捷方式,
	//而并不会删掉主体文件, 详情自己看下面的说明:
	/*
		执行unlink()函数并不一定会真正的删除文件,
		它先会检查文件系统中此文件的连接数是否为1,
		如果不是1说明此文件还有其他链接对象,
		因此只对此文件的连接数进行减1操作.
		若连接数为1, 并且在此时没有任何进程打开该文件, 此内容才会真正地被删除掉.
		在有进程打开此文件的情况下, 则暂时不会删除,
		直到所有打开该文件的进程都结束时文件就会被删除.
	 */
	if(unlink(TEST_LINK_NAME) == -1){// Remove filename
		perror("unlink() failed");
		exit(EXIT_FAILURE);
	}


	exit(EXIT_SUCCESS);
}



//创建symlink链接, 和chmod 改变权限测试
void symlink_chmod(void){
	int fd;



	//1.创建test 目录
	if(mkdir("test", S_IRUSR | S_IWUSR | S_IXUSR) == -1){
		perror("mkdir() failed");
		return ;
	}

	//2.改变程序的运行主目录
	if(chdir("test") == -1){
		perror("chdir() failed");
		return ;
	}

	//3.打开文件
	fd = open("myfile", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if(fd == -1){
		perror("open() failed");
		return;
	}

	//4.关闭文件
	if(close(fd) == -1){
		perror("close() failed");
		return;
	}

	//5.
	if(symlink("myfile", "../mylink") == -1){
		perror("symlink() failed");
		return;
	}

	//6.改变执行权限
	if(chmod("../mylink", S_IRUSR) == -1){
		perror("chmod() failed");
		return;
	}

	return ;
}

