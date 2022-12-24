#include "fs.h"

void my_mkdir(char *dirname) {
    // 1. 判断 dirname 是否合法
    // 2. 打开当前目录的目录文件,查找是否重名
    // 3. 占用一个盘块, 在 FAT 里要写好这个盘块被占用了
    // 4. 占用一个打开文件表项,这是因为一会要在这个目录文件里面写入默认的.和..两个特殊文件目录项
    // 而调用 do_write,是需要先打开这个文件的.
    // 写入.和..两个文件目录项到新建的目录文件里
    // 5. 完毕这个 openfilelist
    // 6. 修改父目录文件的大小,因为添加了一个文件目录项
    // 例 在\a\这个目录下创建了 b 这个目录, 那么 a 这个目录文件的大小要+=sizeof(fcb)
    //判断 dirname 是否合法
    char *fname = strtok(dirname, ".");
    char *exname = strtok(NULL, ".");
    if (exname) {
        printf("Suffix name is not allowed!\n");//不允许输入后缀名
        return;
    }
    char text[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    int fileLen = do_read(currfd, openfilelist[currfd].length, text);
    //text 里的内容就是一个个 fcb
    fcb *fcbPtr = (fcb *) text;
    for (int i = 0; i < (int) (fileLen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbPtr[i].filename) == 0 && fcbPtr->metadata ==
                                                        0) {
            printf("Directory name already exists!\n");//目录名已经存在
            return;
        }
    }
    //在打开文件表里找一个空文件表项
    int fd = get_Free_Openfile();
    if (fd == -1) {
        printf("All open file tables are occupied\n");//打开文件表已全部被占用
        return;
    }
    //在 FAT 里找一个空盘块
    unsigned short int blockNum = getFreeBLOCK();
    if (blockNum == END) {
        printf("The disk has been used up\n");//盘块已经用完
        openfilelist[fd].topenfile = 0;
        return;
    }
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3);
    fat1[blockNum].id = END;
    fat2[blockNum].id = END;
    //在当前目录里面添加一个我们要的目录项
    int i = 0;
    for (; i < (int) (fileLen / sizeof(fcb)); i++) {
        if (fcbPtr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    //修改了 fcb,fcbstate 置 1
    openfilelist[currfd].fcbstate = 1;
    //修改新建的目录项,即 fcb 内容
    //因为现在是在模拟文件系统,我们要先写到临时的 fcb 里,然后用 do_write 转写到磁盘里
    fcb *fcbtmp = (fcb *) malloc(sizeof(fcb));
    fcbtmp->metadata = 0;
    time_t rawtime = time(NULL);
    struct tm *time = localtime(&rawtime);
    fcbtmp->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 +
                   (time->tm_mday);
    fcbtmp->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec)
                                                                  / 2;
    strcpy(fcbtmp->filename, dirname);
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = blockNum;
    fcbtmp->length = 2 * sizeof(fcb);
    fcbtmp->free = 1;
    //用 do_write 把 fcbtmp 写入到目录文件里
    do_write(currfd, (char *) fcbtmp, sizeof(fcb), 1);
    //设置打开文件表项
    openfilelist[fd].metadata = 0;
    openfilelist[fd].filePtr = 0;
    openfilelist[fd].date = fcbtmp->date;
    openfilelist[fd].time = fcbtmp->time;
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].exname, "di");
    strcpy(openfilelist[fd].filename, dirname);
    openfilelist[fd].fcbstate = 0;
    openfilelist[fd].first = fcbtmp->first;
    openfilelist[fd].free = fcbtmp->free;
    openfilelist[fd].length = fcbtmp->length;
    openfilelist[fd].topenfile = 1;
    //在对应的盘块里添加两个特殊的目录.和..
    //同理这里也不能直接写进 fcbPtr 里,只能用 fcbTmp 写进内存里,然后用 do_write转写进文件
    fcbtmp->metadata = 0;
    fcbtmp->date = fcbtmp->date;
    fcbtmp->time = fcbtmp->time;
    strcpy(fcbtmp->filename, ".");
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = blockNum;
    fcbtmp->length = 2 * sizeof(fcb);
    do_write(fd, (char *) fcbtmp, sizeof(fcb), 1);
    //复制..目录
    fcb *fcbtmp2 = (fcb *) malloc(sizeof(fcb));
    memcpy(fcbtmp2, fcbtmp, sizeof(fcb));
    strcpy(fcbtmp2->filename, "..");
    fcbtmp2->first = openfilelist[currfd].first;
    fcbtmp2->length = openfilelist[currfd].length;
    fcbtmp2->date = openfilelist[currfd].date;
    fcbtmp2->time = openfilelist[currfd].time;
    do_write(fd, (char *) fcbtmp2, sizeof(fcb), 1);
    my_close(fd);
    //更新本 currfd 目录文件的 fcb
    fcbPtr = (fcb *) text;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].fcbstate = 1;
    free(fcbtmp);
    free(fcbtmp2);
}