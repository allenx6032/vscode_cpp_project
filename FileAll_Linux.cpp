//#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// enum
// {
//     DT_UNKNOWN = 0,         //未知类型
//     DT_FIFO = 1,            //first in, first out 类似于管道, 有名管道
//     DT_CHR = 2,             //字符设备文件
//     DT_DIR = 4,             //目录
//     DT_BLK = 6,             //块设备文件
//     DT_REG = 8,             //普通文件
//     DT_LNK = 10,            //连接文件
//     DT_SOCK = 12,           //套接字类型
//     DT_WHT = 14             //
// };
//
// struct dirent
// {
//    long d_ino; /* inode number 索引节点号 */
//    off_t d_off; /* offset to this dirent 在目录文件中的偏移 */
//    unsigned short d_reclen; /* length of this d_name 文件名长 */
//    unsigned char d_type; /* the type of d_name 文件类型 */
//    char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长255字符 */
// }
//
// struct __dirstream
// {
// void *__fd; /* `struct hurd_fd' pointer for descriptor.   */
// char *__data; /* Directory block.   */
// int __entry_data; /* Entry number `__data' corresponds to.   */
// char *__ptr; /* Current pointer into the block.   */
// int __entry_ptr; /* Entry number `__ptr' corresponds to.   */
// size_t __allocation; /* Space allocated for the block.   */
// size_t __size; /* Total valid data in the block.   */
// __libc_lock_define (, __lock) /* Mutex lock for this structure.   */
// };
//
// typedef struct __dirstream DIR;


#define FILETXTCNT 100 //文件数量
#define FILENAMELEN 64 //文件名称
 
//存放要解析的基因文件名称
char g_cFileList[FILETXTCNT][FILENAMELEN]; //最多100个文件，每个文件名64字节
 
int GetGeneFile(const char *_pcDir)
{	
	int iFileCnt = 0;
	DIR *dirptr = NULL;
	struct dirent *dirp;
	char cFileTmp[64] = {0};
 
	if((dirptr = opendir(_pcDir)) == NULL)//打开一个目录
	{
		return 0;
	}
	while ((dirp = readdir(dirptr)) != NULL)
	{
		memset(cFileTmp, 0, 64);
		if ((0 == strncasecmp(dirp->d_name, ".", 1)) || (0 != strcmp(strchr(dirp->d_name, '.'), ".txt")))//过滤掉这个文件
		{
			continue;
		}
		
		sprintf(cFileTmp, "%s/%s", _pcDir, dirp->d_name);
		strcpy(g_cFileList[iFileCnt], cFileTmp);
		iFileCnt++;
	}
	closedir(dirptr);
	
	return iFileCnt;
}

int main()
{
	// DIR *d = opendir("D:/");
	//  struct dirent *entry = NULL;
	//  if (!d)
	// {
	// 	 printf("open dir [D:/] failed.\n");
	// 	 return -1;
	// }
	//  else
	// {
	// 	for (entry = readdir(d); entry != NULL; entry = readdir(d))
	// 	{
	// 		if (!strcmp(entry->d_name, "."))
	// 		continue;
	// 		if (!strcmp(entry->d_name, ".."))
	// 		continue;
	// 		if (DT_REG == entry->d_type)
	// 		{
	// 			 if (entry->d_name[0] == '.')
	// 			 continue;
	// 			 printf("list file [%s]\n", entry->d_name);
	// 		}
	// 		else
	// 		{
	// 			printf("file [%s] is %d\n", entry->d_name, entry->d_type);
	// 		}
	// 	}
	// }
	
	GetGeneFile("D:/");
	getchar();
	return 0;
}