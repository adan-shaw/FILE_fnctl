//111111111111111111111111111111111111111111111111111111111111111111111111
//格式测试:
//邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵邵
//起始日期:
//完成日期:
//************************************************************************
//修改日志:

//, , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , , ,, , ,

//编译:
//g++ -o x ./utime_chown.cpp -g3


//改变文件访问时间, 修改时间, 属主


#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <utime.h>
#include <stdlib.h>
#include <unistd.h>

#include <pwd.h>
#include <grp.h>
//#include "ugid_functions.h"//Declarations of userIdFromName()
//                           //and groupIdFromName()



//1.根据'用户id' 获取'用户名'失败返回NULL
char *userNameFromId(uid_t uid);

//2.安全地根据用户名, 获取用户id, 失败返回-1
uid_t userIdFromName(const char *name);

//3.根据'组id' 获取'组名'失败返回NULL
char *groupNameFromId(gid_t gid);

//4.安全地获取'组id' by name, 失败返回-1
gid_t groupIdFromName(const char *name);

//5.修改属主测试
void chown_test(void);

//6.先提取访问时间, 然后修改访问时间, 企图忽略本次访问时间记录?
void utime_test(void);

//7.
void utimes_test(void);



int main(void){


}



//1.根据'用户id' 获取'用户名'失败返回NULL
char *userNameFromId(uid_t uid){
	struct passwd *pwd;

	pwd = getpwuid(uid);
	return (pwd == NULL) ? NULL : pwd->pw_name;
}



//2.安全地根据用户名, 获取用户id, 失败返回-1
uid_t userIdFromName(const char *name){
	struct passwd *pwd;
	uid_t u;
	char *endptr;



	if(name == NULL || *name == '\0')
		return -1;//错误字符串指针, 直接返回.

	u = strtol(name, &endptr, 10);//将字符串转换为long
	if(*endptr == '\0')
		return u;//转换后, 如果返回空串, 直接返回??

	pwd = getpwnam(name);
	if(pwd == NULL){
		perror("getpwnam() failed");
		return -1;
	}

	return pwd->pw_uid;
}



//3.根据'组id' 获取'组名'失败返回NULL
char *groupNameFromId(gid_t gid){
	struct group *grp;

	grp = getgrgid(gid);
	return (grp == NULL) ? NULL : grp->gr_name;
}



//4.安全地获取'组id' by name, 失败返回-1
gid_t groupIdFromName(const char *name){
	struct group *grp;
	gid_t g;
	char *endptr;



	if(name == NULL || *name == '\0')
		return -1;//错误字符串指针, 直接返回.

	g = strtol(name, &endptr, 10);//将字符串转换为long
	if(*endptr == '\0')
		return g;//转换后, 如果返回空串, 直接返回??

	grp = getgrnam(name);
	if(grp == NULL){
		perror("getgrnam() failed");
		return -1;
	}

	return grp->gr_gid;
}



//5.修改属主测试
void chown_test(void){
	const char* powner_id = "adan";
	const char* powner_gid = "adan";
	const char* filename = "utime_chown.cpp";
	uid_t uid;
	gid_t gid;



	uid = userIdFromName(powner_id);
	if(uid == -1){
		printf("userIdFromName(%s) failed\n",powner_id);
		return;
	}

	gid = groupIdFromName(powner_gid);
	if(gid == -1){
		printf("groupIdFromName(%s) failed\n",powner_gid);
		return;
	}


	if(chown(filename, uid, gid) == -1) {
		perror("chown() failed");
		return;
	}

	return;
}



//6.先提取访问时间, 然后修改访问时间, 企图忽略本次访问时间记录?
void utime_test(void){
	const char *pathname = "utime_chown.cpp";
	struct stat sb;
	struct utimbuf utb;



	//获取当前的file stat()
	if(stat(pathname, &sb) == -1){
		perror("stat() failed");
		return ;
	}

	//将当前的file stat()中的访问时间,当成是新的访问时间,写入到file 属性中
	utb.actime = sb.st_atime;				// Leave access time unchanged
	utb.modtime = sb.st_atime;			// Make modify time same as access time
	if(utime(pathname, &utb) == -1){// Update file times
		perror("utime() failed");
		return ;
	}

	return;
}



//7.
void utimes_test(void){
	struct stat sb;
	struct timeval tv[2];
	const char *pathname = "utime_chown.cpp";



	//获取当前的file stat()
	if(stat(pathname, &sb) == -1){
		perror("stat() failed");
		return ;
	}

	tv[0].tv_sec = sb.st_atime;// Leave atime seconds unchanged
	tv[0].tv_usec = 223344;		// Change microseconds for atime
	tv[1].tv_sec = sb.st_atime;// mtime seconds == atime seconds
	tv[1].tv_usec = 667788;		// mtime microseconds

	if(utimes(pathname, tv) == -1){
		perror("utimes() failed");
		return ;
	}

	return;
}
