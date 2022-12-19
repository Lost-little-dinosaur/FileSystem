#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define BLOCKSIZE 1024 //磁盘块大小
#define END 65535
#define SIZE 1024000 //虚拟磁盘空间大小
#define FREE 0
#define ROOTBLOCKNUM 2  //根目录区所占盘块总数
#define MAXOPENFILE 10  //最多同时打开文件数
#define MAX_TEXT_SIZE  10000        //允许读入的最大文件大小
#define FILENAME  "FileSys.txt"     // 文件系统名

typedef struct FCB {
    char filename[8]; //文件名
    char exname[4]; //文件扩展名
    unsigned char metadata; // 文件属性字段 1 表示数据文件，0 表示目录文件
    unsigned char attribute;
    //文件属性字段：0表示目录文件，1表示数据文件
    unsigned short time; //文件创建时间
    unsigned short date; //文件创建日期
    unsigned short first;
    //文件起始盘块号
    unsigned long length;
    //文件长度(字节数)
    char free;
    //表示目录项是否为空，0表示空，1表示已分配
} fcb;


typedef struct FAT {
    unsigned short id;
} fat;

typedef struct USEROPEN {
    char filename[8];
    //文件名(可以存路径，openfilelist[1]就是当前目录，它里面存的是路径)
    char exname[3]; //文件扩展名
    unsigned char attribute;
    //文件属性字段：0表示目录文件，1表示数据文件
    unsigned short time; //文件创建时间
    unsigned short date; //文件创建日期
    unsigned short first;
    //文件起始盘块号
    unsigned long length;
    char dir[80];
    int count;
    //文件长度(字节数)
    char free;
    unsigned char metadata; // 文件属性字段 1 表示数据文件，0 表示目录文件
    //表示目录项是否为空，0表示空，1表示已分配
    int dirno;
    //相应打开文件的目录项在父目录文件中的盘块号
    int diroff;
    //相应打开文件的目录项在父目录文件的dirno盘块中的目录项序号
    char fcbstate;
    //是否修改了文件的FCB内容，如果修改了置为1，否则为0
    char topenfile;
    //表示该用户打开表项是否为空，若值为0，表示为空，否则表示已被某文件占据
    int filePtr;
    // 读写指针在文件中的位置
} useropen;

typedef struct BLOCK0 {//引导块的内容
    char magic_number[8]; // 文件系统的魔数
    char information[200]; //描述信息
    unsigned short root; //root指针指向文件系统的根目录
    unsigned char *startBlock; //startBlock指针指向文件系统中第一个可用的磁盘块
} block0;


void startsys(); // 进入文件系统
void my_format(); // 磁盘格式化函数
void my_cd(char *dirname); // 用于更改当前目录
void my_mkdir(char *dirname); // 创建子目录
void my_rmdir(char *dirname); // 删除子目录
void my_ls(); // 显示目录中的内容
void my_create(char *filename); // 创建文件
void my_rm(char *filename); // 删除文件
int my_open(char *filename); // 打开文件
int my_close(int fd); // 关闭文件
int my_write(int fd); // 写文件
int do_write(int fd, char *text, int len, char wstyle);// 实际写文件
int my_read(int fd, int len); // 读文件
int do_read(int fd, int len, char *text); // 实际读文件
void my_exitsys(); // 退出文件系统
unsigned short getFreeBLOCK(); // 获取一个空闲的磁盘块
int get_Free_Openfile(); // 获取一个空闲的文件打开表项
int find_father_dir(int fd); // 寻找一个打开文件的父目录打开文件
void show_help(); // 获取命令帮助
void error(char *command); // 输出错误信息


unsigned char *myvhard;

unsigned char *v_addr0; // 指向虚拟磁盘的起始地址

useropen openfilelist[MAXOPENFILE]; //用户打开文件表数组

char currentdir[80];

int currfd; // 记录当前目录的文件描述符 fd

int curdir;

unsigned char *startp;

unsigned char buffer[SIZE]; // 缓冲区