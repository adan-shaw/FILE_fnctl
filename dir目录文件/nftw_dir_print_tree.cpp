//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./nftw_dir_print_tree.cpp -g3


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ftw.h>



#define _XOPEN_SOURCE 600 // Get nftw() and S_IFSOCK declarations



//打印正确的shell使用方式
void usageError(const char *progName, const char *msg){
	if(msg != NULL)//如果有附加消息, 则先打印附加消息
		fprintf(stderr, "%s\n", msg);

	fprintf(stderr, "Usage: %s [-d] [-m] [-p] [directory-path]\n", progName);
	fprintf(stderr, "\t-d Use FTW_DEPTH flag\n");
	fprintf(stderr, "\t-m Use FTW_MOUNT flag\n");
	fprintf(stderr, "\t-p Use FTW_PHYS flag\n");
	return;
}



// Function called by nftw() - 识别并打印文件类型!!
int dirTree(const char *pathname, \
		const struct stat *sbuf, int type,struct FTW *ftwb){
	switch(sbuf->st_mode & S_IFMT){// Print file type - 打印文件类型
	case S_IFREG:  printf("-"); break;
	case S_IFDIR:  printf("d"); break;
	case S_IFCHR:  printf("c"); break;
	case S_IFBLK:  printf("b"); break;
	case S_IFLNK:  printf("l"); break;
	case S_IFIFO:  printf("p"); break;
	case S_IFSOCK: printf("s"); break;
	default:       printf("?"); break;//Should never happen (on Linux)
																		//未知文件类型
	}

	printf(" %s  ",
			(type == FTW_D)  ? "D  " : (type == FTW_DNR) ? "DNR" :
			(type == FTW_DP) ? "DP " : (type == FTW_F)   ? "F  " :
			(type == FTW_SL) ? "SL " : (type == FTW_SLN) ? "SLN" :
			(type == FTW_NS) ? "NS " : "  ");

	if(type != FTW_NS)
		printf("%7ld ", (long) sbuf->st_ino);
	else
		printf("        ");

	printf(" %*s", 4 * ftwb->level, "");   // Indent suitably
	printf("%s\n",  &pathname[ftwb->base]);// Print basename
	return 0;                              // Tell nftw() to continue
																				 // 总是不终止nftw()的继续运行
}



//遍历指定目录, 打印出文件树
int main(int argc, char *argv[]){
	int flags, opt;



	flags = 0;
	while ((opt = getopt(argc, argv, "dmp")) != -1){
		switch (opt){
		case 'd': flags |= FTW_DEPTH;   break;
		case 'm': flags |= FTW_MOUNT;   break;
		case 'p': flags |= FTW_PHYS;    break;
		default: { usageError(argv[0], NULL);exit(EXIT_FAILURE);}//未知命令
		}
	}

	//shell 使用格式错误
	if(argc > optind + 1){
		usageError(argv[0], NULL);
		exit(EXIT_FAILURE);
	}

	//遍历并打印文件树
	if(nftw((argc > optind) ? argv[optind] : ".", dirTree, 10, flags) == -1){
		perror("nftw() failed");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
