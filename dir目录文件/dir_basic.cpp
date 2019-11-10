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
//g++ -o x ./dir_basic.cpp -g3


//dir api 集合
/*

int							alphasort(const struct dirent**, const struct dirent**);
int							closedir(DIR *);
int							dirfd(DIR *);
DIR							*fdopendir(int);
DIR							*opendir(const char *);
struct dirent *readdir(DIR *);
int							readdir_r(DIR *restrict, \
								struct dirent *restrict, struct dirent **restrict);
void						rewinddir(DIR *);
int							scandir(const char *, struct dirent ***,\
								int (*)(const struct dirent *), \
								int (*)(const struct dirent**, const struct dirent**));
void						seekdir(DIR *, long);
long						telldir(DIR *);

*/



#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h> // for 原子操作O_RDONLY 之类的
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

#include <limits.h>
#include <dirent.h>//for readdir()
#include <stddef.h>

#include <libgen.h>



//1.dir基础测试
void dir_basic_test(void);


//2.1 遍历某个目录下的所有文件-线程安全版本
void listFiles_r(const char *dirpath);

//2.2 遍历某个目录下的所有文件
//		因为内部使用了静态数据,所以readdir()不是线程安全函数
void listFiles(const char *dirpath);



//测试主函数
int main(void){
	//1.dir基础测试
	dir_basic_test();
	printf("\n");



	//2.读取目录(均以getdents() 为基础, 但为纳入SUSv3)
	//读取目录流DIR*
	//	DIR* opendir(char* path)
	//	DIR* fdopendir(int fd)

	//解析目录流DIR*(只保留demo function)
	printf("2.1 遍历某个目录下的所有文件-线程安全版本:\n");
	listFiles_r(".");
	printf("\n\n");

	printf("2.2 遍历某个目录下的所有文件:\n");
	listFiles(".");
	printf("\n\n");


	//重置DIR* 目录流的pos 游标
	//(readdir() 每次读取, 都会移动一次目录流pos游标)
	//void rewinddir(DIR*)

	//void closedir(DIR*)//关闭目录流



	//3.遍历文件树 passed()

	return 0;
}



//1.dir基础测试
void dir_basic_test(void){
	int tmp;
	char cwdbuf[4096];
	char *ptmp;



	printf("1.dir基础测试\n");
	//1.重命名-文件
	tmp = rename("./x","./x_new");
	if(tmp == -1){
		perror("rename() failed");
		return ;
	}


	//2.创建link(目标, ‘生成的link 存放位置’和名称)
	tmp = link("./x_new","./x");
	if(tmp == -1){
		perror("link() failed");
		return;
	}


	//3.移除link
	tmp = unlink("./x");
	if(tmp == -1){
		perror("unlink() failed");
		return;
	}


	//4.创建文件夹
	tmp = mkdir("./.myMkdir",0600);
	if(tmp == -1){
		perror("mkdir() failed");
		return;
	}


	//5.移除文件夹
	tmp = rmdir("./.myMkdir");
	if(tmp == -1){
		perror("rmdir() failed");
		return;
	}


	//6.删除-文件
	tmp = remove("./x_new");
	if(tmp == -1){
		perror("remove() failed");
		return;
	}


	//7.1: 获取进程当前的工作目录
	if(getcwd(cwdbuf,sizeof(cwdbuf)) == NULL){//线程安全版本
		perror("getcwd() failed");
		return;
	}
	//getwd(cwdbuf);//非线程安全版本
	printf("进程当前的工作目录:\n%s\n\n",cwdbuf);

	//直接返回字符串版本
	//strncpy(cwdbuf,get_current_dir_name(),sizeof(cwdbuf));
	ptmp = get_current_dir_name();
	printf("进程当前的工作目录2:\n%s\n\n",ptmp);




	//7.2: 更改进程当前的工作目录
	chdir(".");
	//fchdir()

	//7.3：更改进程根目录
	chroot(".");



	//8.1 获取文件名(提取文件名, 不包括路径名)
	//char *basename(char *);

	//8.2 获取文件的路径名(提取路径名, 不包括文件名)
	//char *dirname(char *);

	return ;
}



//2.1 遍历某个目录下的所有文件-线程安全版本
void listFiles_r(const char *dirpath){
	DIR *dirp;
	struct dirent *result, *entryp;
	int nameMax;



	//1.获取路径的特定配置信息, 一般默认_PC_NAME_MAX=255(路径名最大长度)
	nameMax = pathconf(dirpath, _PC_NAME_MAX);
	if(nameMax == -1){
		perror("pathconf() failed");
		return;
	}


	//2.分配所有struct dirent 所需的内存
	entryp = (struct dirent *)malloc(\
			offsetof(struct dirent, d_name) + nameMax + 1);
	if(entryp == NULL){
		perror("malloc() fail");
		return;
	}


	//3.获取目录流DIR*
	dirp = opendir(dirpath);
	if(dirp == NULL) {
		perror("opendir() fail");
		free(entryp);
		return;
	}


	//4.解析目录流DIR*
	for(;;) {
		errno = readdir_r(dirp, entryp, &result);
		if(errno != 0){
			perror("readdir_r() fail");
			break;
		}

		if(result == NULL)
			break;

		//跳过目录自身和父目录
		if(strcmp(entryp->d_name, ".") == 0 ||
					strcmp(entryp->d_name, "..") == 0)
			continue;

		//打印目录内的所有信息
		printf("%s\n", entryp->d_name);
	}


	//5.关闭目录流DIR*
	if(closedir(dirp) == -1)
		perror("closedir() fail");

	free(entryp);

	return;
}



//2.2 遍历某个目录下的所有文件
//		因为内部使用了静态数据,所以readdir()不是线程安全函数
void listFiles(const char *dirpath){
	DIR *dirp;
	struct dirent *dp;



	//1.获取目录流DIR*
	dirp = opendir(dirpath);
	if(dirp  == NULL){
		perror("opendir() fail");
		return;
	}


	//2.解析目录流DIR*
	for(;;){
		dp = readdir(dirp);
		if(dp == NULL)
			break;

		//跳过目录自身和父目录??
		//if(strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
			//continue;

		printf("%s\n", dp->d_name);//打印目录名

		//给所有文件, 加上固定的前缀
		//(请谨慎操作, linux 文件操作一定程度上很难复原!!)
		/*
		char *target_string = "S01E";\
			//给当前文件夹下所有文件, 加上x前缀
		char buf[256];
		memset(buf,'\0',256);
		strncpy(buf,target_string,256);
		strncat(buf,dp->d_name,sizeof(buf));
		rename(dp->d_name,buf);
		*/
	}


	//3.关闭目录流DIR*
	if(closedir(dirp) == -1)
		perror("closedir() fail");

	return;
}
