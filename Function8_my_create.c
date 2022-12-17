#include "fs.h"

int my_create(char *filename) {
    // ① 为新文件分配一个空闲打开文件表项，如果没有空闲表项则返回-1，并显示错误信息；
    // ② 若新文件的父目录文件还没有打开，则调用 my_open()打开；若打开失败，则释放①中为新建文件分配的空闲文件打开表项，返回 - 1，并显示错误信息；
    // ③ 调用 do_read()读出该父目录文件内容到内存，检查该目录下新文件是否重名，若重名则释放①中分配的打开文件表项，并调用my_close()关闭②中打开的目录文件；然后返回 - 1，并显示错误信息；
    // ④ 检查 FAT 是否有空闲的盘块，如有则为新文件分配一个盘块，否则释放①中分配的打开文件表项，并调用my_close()关闭②中打开的目录文件；返回 - 1，并显示错误信息；
    // ⑤ 在父目录中为新文件寻找一个空闲的目录项或为其追加一个新的目录项;需修改该目录文件的长度信息，并将该目录文件的用户打开文件表项中的fcbstate置为1；
    // ⑥ 准备好新文件的 FCB 的内容，文件的属性为数据文件，长度为 0，以覆盖写方式调用do_write()将其填写到⑤中分配到的空目录项中；
    // ⑦ 为新文件填写①中分配到的空闲打开文件表项，fcbstate 字段值为 0，读写指针值为0；
    // ⑧ 调用 my_close()关闭②中打开的父目录文件；
    // ⑨ 将新文件的打开文件表项序号作为其文件描述符返回。
    char *fname = strtok(filename, ".");//分解字符串 filename 为一组字符串，"."为分隔符。
    char *exname = strtok(NULL, ".");
    if (strcmp(fname, "") == 0) {
        printf("请输入文件名!\n");
        return -1;
    }
    if (!exname) {
        printf("请输入后缀名!\n");
        return -1;
    }
    if (openfilelist[currfd].metadata == 1) {
        printf("数据文件下不允许使用 create\n");
        return -1;
    }
    //读取 currfd 对应的文件
    openfilelist[currfd].filePtr = 0;
    char buf[MAX_TEXT_SIZE];
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb *fcbPtr = (fcb *) (buf);
    //看看有没有重名文件
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (strcmp(fcbPtr->filename, filename) == 0 &&
            strcmp(fcbPtr->exname, exname) == 0) {
            printf("已有同名文件存在!\n");
            return -1;
        }
    }
    //寻找空的 fcb 块
    fcbPtr = (fcb *) (buf);
    fcb *debug = (fcb *) (buf);
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (fcbPtr->free == 0)break;
    }
    //取一个盘块
    int blockNum = getFreeBLOCK();
    if (blockNum == -1) {
        return -1;
    }
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    fat1[blockNum].id = END;
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3);
    memcmp(fat2, fat1, BLOCKSIZE * 2);
    //往 fcb 里写信息
    strcpy(fcbPtr->filename, filename);
    strcpy(fcbPtr->exname, exname);
    time_t rawtime = time(NULL);
    struct tm *time = localtime(&rawtime);
    fcbPtr->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 +
                   (time->tm_mday);
    fcbPtr->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 +
                   (time->tm_sec) / 2;
    fcbPtr->first = blockNum;
    fcbPtr->free = 1;
    fcbPtr->length = 0;
    fcbPtr->metadata = 1;
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    //修改当前目录文件的.目录项的长度
    fcbPtr = (fcb *) buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].fcbstate = 1;
}
