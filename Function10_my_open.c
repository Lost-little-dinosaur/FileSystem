#include "fs.h"


int my_open(char *filename) {
    //把当前目录文件读取到 buf 里,buf 里面就是一个个 fcb 了,在这些 fcb 里寻找匹配上filename 的数据文件
    //然后从 openfilelist 里取出一个打开文件表项,把这个数据文件的信息写进去,切换currfd = fd就算是打开文件了
    //把当前目录文件读取到 buf 里
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    char *fname = strtok(filename, ".");
    char *exname = strtok(NULL, ".");
    if (!exname) {
        printf("请输入后缀名\n");
        return -1;
    }
    int i;
    fcb *fcbPtr = (fcb *) buf;
    //寻找此文件
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (strcmp(fcbPtr->filename, fname) == 0 &&
            strcmp(fcbPtr->exname, exname) == 0 && fcbPtr->metadata == 1) {
            break;
        }
    }
    if (i == (int)(openfilelist[currfd].length / sizeof(fcb))){
        printf("不存在此文件!\n");
        return -1;
    }
    //为它创建一个打开文件表项
    int fd = get_Free_Openfile();
    if (fd == -1) {
        printf("用户打开文件表已经用满\n");
        return -1;
    }
    openfilelist[fd].metadata = 1;
    openfilelist[fd].filePtr = 0;
    openfilelist[fd].date = fcbPtr->date;
    openfilelist[fd].time = fcbPtr->time;
    strcpy(openfilelist[fd].exname, exname);
    strcpy(openfilelist[fd].filename, fname);
    openfilelist[fd].length = fcbPtr->length;
    openfilelist[fd].first = fcbPtr->first;
//    strcpy(openfilelist[fd].dir, (string(openfilelist[currfd].dir) +
//                                  string(filename)).c_str());
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    openfilelist[fd].free = 1;
    openfilelist[fd].topenfile = 1;
    openfilelist[fd].fcbstate = 0;
    currfd = fd;
    return 1;
}
