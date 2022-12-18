#include "fs.h"

void my_rm(char *filename) {
    //例 现在在\a\目录下
    //1. 读取当前目录的目录文件(a)到 buf 里(buf 里是一个个 fcb)
    //2. 在 buf 里寻找匹配 filename 的数据文件
    //3. 清空这个文件占据的盘块(也就是释放它占据的 fat,并且备份)
    //4. 在父目录文件里,删除 filename 对应的 fcb (a 里删除 b 的 fcb)
    //5. 更新父目录文件的长度(a 的目录文件长度)
    char *fname = strtok(filename, ".");
    char *exname = strtok(NULL, ".");
    if (!exname) {
        printf("请输入后缀名!\n");
        return;
    }
    if (strcmp(exname, "di") == 0) {
        printf("不能删除目录项\n");
        return;
    }
    //读取 currfd 对应的目录文件到 buf
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb *fcbPtr = (fcb *) buf;
    //寻找叫这个名字的文件目录项
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (strcmp(fcbPtr->filename, fname) == 0 && strcmp(fcbPtr->exname, exname) == 0) {
            break;
        }
    }
    if (i == (int)(openfilelist[currfd].length / sizeof(fcb))){
        printf("没有这个文件\n");
        return;
    }
    //清空这个目录项占据的 FAT
    int blockNum = fcbPtr->first;
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    int next = 0;
    while (1) {
        next = fat1[blockNum].id;
        fat1[blockNum].id = FREE;
        if (next != END) {
            blockNum = next;
        } else {
            break;
        }
    }
    //备份 fat2
    fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3);
    memcpy(fat2, fat1, sizeof(fat));
    //修改这个 fcb 为空
    fcbPtr->date = 0;
    fcbPtr->time = 0;
    fcbPtr->exname[0] = '\0';
    fcbPtr->filename[0] = '\0';
    fcbPtr->first = 0;
    fcbPtr->free = 0;
    fcbPtr->length = 0;
    //写到磁盘上去, 更新 fcb 内容为空
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].length -= sizeof(fcb);
    //更新.目录项的长度
    fcbPtr = (fcb *) buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].length -= sizeof(fcb);
    //更新.目录项的长度
    fcbPtr = (fcb *) buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].fcbstate = 1;
}
