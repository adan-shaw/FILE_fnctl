//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
///***********************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,


//编译:
//g++ -o x ./fork_file_sharing.cpp -g3

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>



int main(int argc, char *argv[]){
	int fd, flags, tmp;
	char str_tmp[] = "/tmp/testXXXXXX";



	//设置stdout FILE 标准流的缓冲区大小
	setbuf(stdout, NULL);// Disable buffering of stdout


	//创建临时文件
	fd = mkstemp(str_tmp);
	if(fd == -1){
		perror("mkstemp() failed");
		return -1;
	}


	printf("File offset before fork(): %lld\n",
			(long long) lseek(fd, 0, SEEK_CUR));

	flags = fcntl(fd, F_GETFL);
	if(flags == -1){
		perror("fcntl() - F_GETFL failed");
		return -1;
	}

	printf("O_APPEND flag before fork() is: %s\n",
			(flags & O_APPEND) ? "on" : "off");



	tmp = fork();
	switch(tmp){

	case -1:
		perror("fork() failed");
		break;

	case 0:// Child: change file offset and status flags
		if(lseek(fd, 1000, SEEK_SET) == -1){
			perror("lseek() failed");
			break;
		}

		flags = fcntl(fd, F_GETFL);// Fetch current flags
		if(flags == -1){
			perror("fcntl() - F_GETFL failed");
			break;
		}
		flags |= O_APPEND;// Turn O_APPEND on
		if(fcntl(fd, F_SETFL, flags) == -1){
			perror("fcntl() - F_SETFL failed");
			break;
		}
		break;

	default:// Parent: can see file changes made by child
		if(wait(NULL) == -1){
			perror("wait() failed");// Wait for child exit
			break;
		}
		printf("Child has exited, Parent process going on\n");


		printf("File offset in parent: %lld\n",
				(long long) lseek(fd, 0, SEEK_CUR));


		flags = fcntl(fd, F_GETFL);
		if(flags == -1){
			perror("fcntl() - F_GETFL failed");
			break;
		}
		printf("O_APPEND flag in parent is: %s\n",
				(flags & O_APPEND) ? "on" : "off");
		break;

	}//switch() end

	return 0;
}
