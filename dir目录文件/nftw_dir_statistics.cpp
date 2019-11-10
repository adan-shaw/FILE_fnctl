//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./nftw_dir_statistics.cpp -g3


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ftw.h>



#define _XOPEN_SOURCE 600 // Get nftw() and S_IFSOCK declarations


//文件遍历时, 统计用的变量
static int numReg = 0, numDir = 0, numSymLk = 0, numSocket = 0,
           numFifo = 0, numChar = 0, numBlock = 0,
           numNonstatable = 0;



//遍历函数时调用的'子回调函数', 用于统计各类文件的数量
static int countFile(const char *path,\
		const struct stat *sb, int flag, struct FTW *ftwb){
	if(flag == FTW_NS){
		numNonstatable++;
		return 0;
	}

	//遍历时, 判断各类文件类型的循环.
	//(nftw() 每遍历一个文件, 这个'子回调函数'就会被调用一次!)
	switch (sb->st_mode & S_IFMT){
		case S_IFREG:  numReg++;    break;
		case S_IFDIR:  numDir++;    break;
		case S_IFCHR:  numChar++;   break;
		case S_IFBLK:  numBlock++;  break;
		case S_IFLNK:  numSymLk++;  break;
		case S_IFIFO:  numFifo++;   break;
		case S_IFSOCK: numSocket++; break;
	}

	// Always tell nftw() to continue(总是不终止nftw()遍历的继续进行)
	// 总是返回0
	return 0;
}



//打印单项统计数据的函数
inline void printStats(const char *msg, int num, int numFiles){
	printf("%-15s   %6d %6.1f%%\n", msg, num, num * 100.0 / numFiles);
	return;
}



//遍历指定目录, 并打印该目录的文件分布情况
int main(int argc, char *argv[]){
	int numFiles; //Total number of files



	//使用方法: ./x . # 对当前目录的文件分布情况进行检索
	//				默认情况下, . 文件也会算一个目录. 总数+1, 目录数量+1
	if(argc != 2 || strcmp(argv[1], "--help") == 0){
		printf("%s dir-path\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	// Traverse directory tree counting files; 遍历文件树
	// don't follow symbolic links. 不遵循符号链接
	// 执行遍历, 并用子回调函数进行文件类型统计, 将统计到的数据存放到static变量中
	if(nftw(argv[1], &countFile, 20, FTW_PHYS) == -1){
		perror("nftw() failed");
		exit(EXIT_FAILURE);
	}

	//统计出文件 + 目录的总量
	numFiles = numReg + numDir + numSymLk + numSocket +
							numFifo + numChar + numBlock + numNonstatable;

	//打印统计结果
	if(numFiles == 0){
		//该目录下, 没有任何文件.
		printf("No files found\n");
		exit(EXIT_SUCCESS);
	}
	else{
		//如果有文件, 打印出统计结果.
		printf("Total files:      %6d\n", numFiles);
		printStats("Regular:", numReg, numFiles);
		printStats("Directory:", numDir, numFiles);
		printStats("Char device:", numChar, numFiles);
		printStats("Block device:", numBlock, numFiles);
		printStats("Symbolic link:", numSymLk, numFiles);
		printStats("FIFO:", numFifo, numFiles);
		printStats("Socket:", numSocket, numFiles);
		printStats("Non-statable:", numNonstatable, numFiles);
	}
	exit(EXIT_SUCCESS);
}
