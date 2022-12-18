#include "fs.h"

int my_close(int fd) {
    if (fd > MAXOPENFILE || fd < 0) { //检查 fd 的有效性，如果无效则返回-1；
        printf("此打开文件不存在\n");
        return -1;
    } else {
        //判断父目录文件是否存在, 不存在-1
        int fatherFd = find_father_dir(fd);
        if (fatherFd == -1) {
            printf("父目录不存在");
            return -1;
        }
        //检查fcbstate字段,如果是 1,则需要将该文件的FCB的内容保存到虚拟磁盘上该文件的目录项
        if (openfilelist[fd].fcbstate == 1) {
            char buf[MAX_TEXT_SIZE];
            do_read(fatherFd, openfilelist[fatherFd].length, buf); //把父目录文件从磁盘中读取到 buf 中
            //更新 fcb 内容
            fcb *fcbPtr = (fcb *) (buf + sizeof(fcb) * openfilelist[fd].diroff); //fcbPtr 指向这个文件的对应 fcb 的位置
            //diroff 相应打开文件的目录项在父目录文件的 dirno 盘块中的目录项序号
            strcpy(fcbPtr->exname, openfilelist[fd].exname); // 文件扩展名
            strcpy(fcbPtr->filename, openfilelist[fd].filename); // 文件名
            fcbPtr->first = openfilelist[fd].first; // 文件起始盘块号
            fcbPtr->free = openfilelist[fd].free; // 表示目录项是否为空，若值为 0，表示空，值为 1，表示已分配
            fcbPtr->length = openfilelist[fd].length; // 文件长度（对数据文件是字节数，对目录文件可以是目录项个数）
            openfilelist[fatherFd].filePtr = 0; // 读写指针在文件中的位置
            fcbPtr->time = openfilelist[fd].time; // 文件创建时间
            fcbPtr->date = openfilelist[fd].date; // 文件创建日期
            fcbPtr->metadata = openfilelist[fd].metadata; // 文件属性字段 1 表示数据文件，0 表示目录文件
            openfilelist[fatherFd].filePtr = openfilelist[fd].diroff * sizeof(fcb);
            do_write(fatherFd, (char *) fcbPtr, sizeof(fcb), 1);
        }
        memset(&openfilelist[fd], 0, sizeof(useropen));
        currfd = fatherFd;
        return fatherFd;
    }
}

