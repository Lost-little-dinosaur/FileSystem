#include "fs.h"


int my_read(int fd, int len) {
    if (fd >= MAXOPENFILE || fd < 0) {
        printf("file does not exist\n");//�ļ�������
        return -1;
    }
    openfilelist[fd].filePtr = 0;
    char text[MAX_TEXT_SIZE] = "\0";
    if (len > openfilelist[fd].length) {
        printf("The read length has exceeded the file size. By default, it will be read to the end of the file.\n");//��ȡ�����Ѿ������ļ���С��Ĭ�϶����ļ�ĩβ.
        len = openfilelist[fd].length;
    }
    do_read(fd, len, text);
    printf("%s\n", text);
    return 1;
}
