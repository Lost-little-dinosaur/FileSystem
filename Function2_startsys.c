#include "fs.h"


//1. 寻找 myfsys.txt 文件,如果存在而且开头是文件魔数,就读取到myvhard,否则创建文件并写入初始化信息
//2. 设置用户打开文件表的第一个表项, 内容为根目录内容, 也就是默认打开根目录
//3. 初始化一堆全局变量
void startsys() {
    // ① 申请虚拟磁盘空间；16
    // ② 使用 c 语言的库函数 fopen()打开 myfsys 文件：若文件存在，则转③；若文件不存在，则创建之，转⑤
    // ③ 使用 c 语言的库函数 fread()读入 myfsys 文件内容到用户空间中的一个缓冲区中，并判断其开始的 8 个字节内容是否为“10101010”（文件系统魔数），如果是，则转④；否则转⑤；
    // ④ 将上述缓冲区中的内容复制到内存中的虚拟磁盘空间中；转⑦
    // ⑤ 在屏幕上显示“myfsys 文件系统不存在，现在开始创建文件系统”信息，并调用 my_format()对①中申请到的虚拟磁盘空间进行格式化操作。转⑥；
    v_addr0 = (unsigned char *) malloc(SIZE); //申请虚拟磁盘空间
    //如果文件不存在或者开头不是文件魔数,都重新创建文件
    printf("Start reading file");//开始读取文件...
    FILE *file;
    if ((file = fopen(FILENAME, "r")) != NULL) {
        fread(buffer, SIZE, 1, file); //将二进制文件读取到缓冲区
        fclose(file);
        if (memcmp(buffer, "10101010", 8) == 0) {
            memcpy(v_addr0, buffer, SIZE);
            printf("Myfsys read successfully!\n");//myfsys 读取成功
        }
            //有文件但是开头不是文件魔数
        else {
            printf("The myfsys file system does not exist. Now start to create the file system\n");//myfsys 文件系统不存在，现在开始创建文件系统
            my_format();
            memcpy(buffer, v_addr0, SIZE);
        }
    } else {
        printf("The myfsys file system does not exist. Now start to create the file system\n");//myfsys 文件系统不存在，现在开始创建文件系统
        my_format();
        memcpy(buffer, v_addr0, SIZE);
    }
    // ⑧ 初始化用户打开文件表，将表项 0 分配给根目录文件使用，并填写根目录文件的相关信息，由于根目录没有上级目录，
    // 所以表项中的 dirno 和 diroff 分别置为 5（根目录所在起始块号）和 0；并将 ptrcurdir 指针指向该用户打开文件表项。
    // ⑨ 将当前目录设置为根目录。
    fcb *root;
    root = (fcb * )(v_addr0 + 5 * BLOCKSIZE);
    strcpy(openfilelist[0].filename, root->filename);
    strcpy(openfilelist[0].exname, root->exname);
    openfilelist[0].metadata = root->metadata;
    openfilelist[0].time = root->time;
    openfilelist[0].date = root->date;
    openfilelist[0].first = root->first;
    openfilelist[0].length = root->length;
    openfilelist[0].free = root->free;
    openfilelist[0].dirno = 5;
    openfilelist[0].diroff = 0;
    strcpy(openfilelist[0].dir, "\\root\\");
    openfilelist[0].filePtr = 0;
    openfilelist[0].fcbstate = 0;
    openfilelist[0].topenfile = 1;
    //初始化全局变量
    //startp 指向数据区的开头
    startp = ((block0 *) v_addr0)->startBlock;
    currfd = 0;
    return;
}