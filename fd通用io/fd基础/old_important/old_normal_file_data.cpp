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
//g++ -o x ./old_normal_file_data.cpp -g3


//关于'通用io'的应用数据读写:
/*
	一般情况下, 如果应用数据长期同步到磁盘, 一般用mmap().
	如果是偶尔保存, 长期打开占用, 也会用FILE 比较妥当.
	'通用io'用来偶尔保存应用数据的可能性较低.(比较少用)
*/
