#include "fs.h"


int my_read(int fd, int len) {
    if (fd >= MAXOPENFILE || fd < 0) {
        printf("�ļ�������\n");
        return -1;
    }
    openfilelist[fd].filePtr = 0;
    char text[MAX_TEXT_SIZE] = "\0";
    if (len > openfilelist[fd].length) {
        printf("��ȡ�����Ѿ������ļ���С��Ĭ�϶����ļ�ĩβ.\n");
        len = openfilelist[fd].length;
    }
    do_read(fd, len, text);
    printf("%s\n", text);
    return 1;
}
