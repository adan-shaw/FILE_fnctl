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
//g++ -o x ./getline.cpp -g3


//其实getline() 函数真的没有fgets() 函数好用！！


//getline() 函数分析:
/*
1.为什么有getline() 函数:
	其实getline() ,fgets() 是差不多的, 没啥特别不同.
	getline()类似于fgets(), 只识别LF, 不会去除LF.
	也就是: 遇到'\0'是不会识别的, 以'\n'为行结束标志.

	不同的是: getline() 可以自动分配内存.
	函数原型如下:
	#include <stdio.h>
	ssize_t getline(char **lineptr, size_t *n, FILE *stream);
	ssize_t getdelim(char **lineptr,size_t *n,int delim,FILE *stream);

	参数分析:
	char **lineptr：
		指向存放该行字符的指针, 如果是NULL, 则有系统帮助malloc,
		请在使用完成后free释放.
	size_t *n：
		如果是由系统malloc的指针, 请填0.
	FILE *stream:
		目标FILE*文件流

	返回值分析:
		如果getline没有读入字符, 将返回false, 可用于判断文件是否结束.



2.getline()与fgets的不同
	getline()函数会自动为读取的字符末尾加一个空字符'\0'.
	fgets(),则不会理会'\0', 而是以'\n'作为结束符号.
	所以你写入的时候不添加'\n',那么fgets()可能会两行连起来读取.



3.getline()与fgets()的性能对比
	打开一个5M的文本, fgets()200ms, getline() 1000ms.
	fgets()性能更优!!



4.编译器区分的差异:
	getline() 是c++ 全局函数.
	在标准c语言中, getline()函数是不存在的.
	在gcc编译器中, 对标准c库进行了扩展, 加入了一个getline()函数.

	C++中读取一行的getline()函数是不读入换行符的,
	而C语言中GCC编译器扩展的getline()函数和fgets()都是是读入换行符的.



5.总结：
	按行读取文件就用c++全局函数std::getline()吧, 简单方便.


不会自动添加'\0'的fgets(), 缓冲区刚开始memset(buf,'0',sizeof(buf));
也不会在后面发现乱码, 证明fgets() 也会补全'\0'结尾, 遇到'\n'之后会结束一行.
并且补全'\0'...那么getline() 到底有什么不一样??

*/



/*

2.readline() 为什么没有对称的putline(), 写入操作的时候怎么办？？
	c 语言FILE流text数据-序列化写入, 为什么fputs() 就足够了??

	写入fputs() 就足够了？ why??
	因为每次缓冲区一般只有一个string 需要写入, 不能写入多个string !!

	但是读取的时候, fgets() 就不够用了, 因为可能会一次读取到两句string !!

*/



#include <stdio.h>
#include <stdlib.h>
#include <string.h>



//getline()测试函数
void getline_test(void);



int main(void){
	getline_test();//getline()测试函数

	exit(EXIT_SUCCESS);
}



//getline()测试函数
void getline_test(void){
	FILE *fp;
	unsigned long int nRead=0, len=0;
	char *line = NULL;



	//用FILE流, 打开文件
	fp = fopen("./test_data", "r");
	if(fp == NULL){
		perror("fopen() failed");
		return;
	}

	//读一行
	nRead = getline(&line, &len, fp);
	printf("fight line:%s",line);
	printf("\n\n");


	//将整个文件按照'行读'的方式, 逐行读取&&打印
	while((nRead = getline(&line, &len, fp)) != -1){
		printf("Retrieved line of length %d :\n", nRead);
		printf("%s", line);
	}


	//getline 返回的是malloc 分配的字符串, 需要手动释放
	//但是只需要释放一次, 缓冲区会自动重复利用
	//做法就是: 默认分配4kb页, 不够大再自动分配, 重复利用, 用完只需释放一次!
	free(line);


	fclose(fp);//关闭文件流

	return;
}
