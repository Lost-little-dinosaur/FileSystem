#include "fs.h"

void my_rmdir(char *dirname) {
    //这里,我们不允许删除非空的目录文件
    //1. 把当前目录的目录文件读取到 buf 里
    //2. 在里面查找有没有匹配 dirname 的目录文件
    //3. 删除这个目录文件占用的所有盘块(也就是把他占用的 fat 全部释放,并且备份到 fat2)
    //4. 更新当前目录文件,把 dirname 对应的 fcb 清除,并且更新当前目录文件的大小
    //例 你在\a\目录下,删除 b 这个目录,那么删除完之后,a 这个目录文件的大小(length)要-=sizeof(fcb)
    // 并且,b 这个目录文件是有一个 fcb 在 a 这个目录文件里的,把这个 fcb 也删掉
    char *fname = strtok(dirname, ".");
    char *exname = strtok(NULL, ".");
    //不允许删除.和..这两个特殊目录文件
    if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0) {
        printf("不能删除%s这个特殊目录项\n", dirname);
        return;
    }
    if (exname) {
        printf("删除目录文件不用输入后缀名!\n");
        return;
    }
    //读取 currfd 对应的目录文件到 buf
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb *fcbPtr = (fcb *) buf;
    //寻找叫这个名字的文件目录项
    for (i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb));
         i++, fcbPtr++) {
        if (strcmp(fcbPtr->filename, fname) == 0 && fcbPtr->metadata == 0) {
            break;
        }
    }
    if (i == (int) (openfilelist[currfd].length / sizeof(fcb))) {
        printf("没有这个目录文件\n");
        return;
    }
    //判断这个目录文件里,清空了没有,我们不允许删除没有清空的目录
    if (fcbPtr->length > 2 * sizeof(fcb)) {
        printf("请先清空这个目录下的所有文件,再删除目录文件\n");
        return;
    }
    //清空这个目录项占据的 FAT
    int blockNum = fcbPtr->first;
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    int next = 0;
    while (1) {
        next = fat1[blockNum].id;
        fat1[blockNum].id = END;
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
    openfilelist[currfd].fcbstate = 1;
}