#include "fs.h"

void my_ls() {
    // ① 调用 do_read()读出当前目录文件内容到内存；
    // ② 将读出的目录文件的信息按照一定的格式显示到屏幕上；
    // ③ 返回。
    if (openfilelist[currfd].metadata == 1) {
        printf("在数据文件里不能使用 ls\n");
        return;
    }
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, (int) openfilelist[currfd].length, buf);
    fcb *fcbPtr = (fcb *) buf;
    printf("name\tsize \ttype\t\tdate\t\ttime\n");
    for (int i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb)); i++) { //fcb大小为28
        if (fcbPtr->free == 1) {
            //目录文件
            //同理,年份占 7 位,月份占 4 位,日期占 5 位
            //小时占 5 位,分钟占 6 位,秒占 5 位
            if (fcbPtr->metadata == 0) {
                printf("%s\t%luB\t<DIR>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       fcbPtr->filename, fcbPtr->length,
                       (fcbPtr->date >> 9) + 2000,
                       (fcbPtr->date >> 5) & 0x000f,
                       (fcbPtr->date) & 0x001f,
                       (fcbPtr->time >> 11),
                       (fcbPtr->time >> 5) & 0x003f,
                       (fcbPtr->time) & 0x001f * 2);
            } else {
                // 普通文件 length - 2 是因为末尾有/n 和/0 两个字符
                unsigned int length = fcbPtr->length;
                if (length != 0)length -= 2;
                printf("%s.%s\t%dB\t<File>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       fcbPtr->filename,
                       fcbPtr->exname,
                       length,
                       (fcbPtr->date >> 9) + 2000,
                       (fcbPtr->date >> 5) & 0x000f,
                       (fcbPtr->date) & 0x001f,
                       (fcbPtr->time >> 11),
                       (fcbPtr->time >> 5) & 0x003f,
                       (fcbPtr->time) & 0x001f * 2);
            }
        }
        fcbPtr++;
    }
}
