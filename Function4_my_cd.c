#include "fs.h"

void my_cd(char *dirname) {
    //总结:
    // 1. 如果当前是目录文件下,那么需要把这个目录文件读取到 buf 里, 然后检索这个文件里的 fcb 有没有匹配 dirname 的目录项(而且必须是目录文件)
    // 如果有,那就在 openfilelist 里取一个打开文件表项,把这个dirname 这个目录文件的 fcb 写进去,然后切换 currfd=fd，这样就算是打开一个目录了
    if (openfilelist[currfd].metadata == 1) { //用户打开文件表的文件属性字段：1 表示数据文件，0 表示目录文件
        printf("该文件不是一个目录文件，而是一个数据文件，无法使用cd命令进入\n");
        return;
    } else {//如果是目录文件
        //寻找目录文件里面有没有匹配的名字, 先把目录文件的信息读取到 buf 里
        char *buf = (char *) malloc(MAXOPENFILE); //申请一个缓冲区，用来存放目录文件的内容，大小为 MAXOPENFILE
        openfilelist[currfd].filePtr = 0; //把文件指针指向文件开头
        do_read(currfd, openfilelist[currfd].length, buf); //目录文件信息读到buf里
        int i;
        fcb *fcbPtr = (fcb *) buf; //把 buf 里的内容强制转换成 fcb 类型
        for (i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb)); i++) { //遍历目录文件里的所有 fcb
            if (strcmp(fcbPtr->filename, dirname) == 0 && fcbPtr->metadata == 0) { //如果找到了匹配的目录文件
                break;
            } else {
                fcbPtr++; //指向下一个 fcb,继续遍历
            }
        }
        if (fcbPtr->exname[0]!='d' || fcbPtr->exname[1]!='i'){ //不允许 cd 非目录文件
            printf("该文件不是一个目录文件，而是一个数据文件，无法使用cd命令进入\n");
            return;
        } else { //如果 cd 了一个目录文件, 那么判断是.还是..还是子文件,如果是子文件则打开这个目录文件到openfilelist里
            if (strcmp(fcbPtr->filename, ".") == 0) { //cd .不会有反应
                return;
            } else if (strcmp(fcbPtr->filename, "..") == 0) {//cd ..需要判断现在是不是根目录, 如果是根目录,不操作, 否则,返回上一层
                if (currfd == 0) {
                    return;
                } else {
                    currfd = my_close(currfd);
                    return;
                }
            } else { //cd 子文件
                int fd = get_Free_Openfile(); //从打开文件表里面找一个空闲的打开文件表项
                if (fd == -1) { //如果没有空闲的打开文件表项或者打开文件表已经满了
                    return;
                } else {
                    // 将这个目录文件的 fcb 对应的信息写进打开文件表项里
                    openfilelist[fd].metadata = fcbPtr->metadata;
                    openfilelist[fd].filePtr = 0;
                    openfilelist[fd].date = fcbPtr->date;
                    openfilelist[fd].time = fcbPtr->time;
                    strcpy(openfilelist[fd].filename,
                           fcbPtr->filename);
                    strcpy(openfilelist[fd].exname, fcbPtr->exname);
                    openfilelist[fd].first = fcbPtr->first;
                    openfilelist[fd].free = fcbPtr->free;
                    openfilelist[fd].length = fcbPtr->length;

                    openfilelist[fd].fcbstate = 0;
                    strcpy(openfilelist[fd].dir,
                           (char *) (strcat(strcat(openfilelist[currfd].dir, dirname), "\\"))); //把当前目录的路径写进打开文件表项里
                    openfilelist[fd].topenfile = 1;
                    openfilelist[fd].dirno = openfilelist[currfd].first;
                    openfilelist[fd].diroff = i;
                    currfd = fd; //切换当前目录
                }
            }
        }
    }
}