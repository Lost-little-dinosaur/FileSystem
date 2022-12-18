#include "fs.h"

void my_ls() {
    // �� ���� do_read()������ǰĿ¼�ļ����ݵ��ڴ棻
    // �� ��������Ŀ¼�ļ�����Ϣ����һ���ĸ�ʽ��ʾ����Ļ�ϣ�
    // �� ���ء�
    if (openfilelist[currfd].metadata == 1) {
        printf("�������ļ��ﲻ��ʹ�� ls\n");
        return;
    }
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    fcb *fcbPtr = (fcb *) buf;
    printf("name\tsize \ttype\t\tdate\t\ttime\n");
    for (int i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb));
         i++) {
        if (fcbPtr->free == 1) {
            //Ŀ¼�ļ�
            //ͬ��,���ռ 7 λ,�·�ռ 4 λ,����ռ 5 λ
            //Сʱռ 5 λ,����ռ 6 λ,��ռ 5 λ
            if (fcbPtr->metadata == 0) {
                printf("%s\t%dB\t<DIR>\t%d/%d/%d\t%02d:%02d:%02d\n",
                       fcbPtr->filename, fcbPtr->length,
                       (fcbPtr->date >> 9) + 2000,
                       (fcbPtr->date >> 5) & 0x000f,
                       (fcbPtr->date) & 0x001f,
                       (fcbPtr->time >> 11),
                       (fcbPtr->time >> 5) & 0x003f,
                       (fcbPtr->time) & 0x001f * 2);
            } else {
                // ��ͨ�ļ� length - 2 ����Ϊĩβ��/n ��/0 �����ַ�
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
