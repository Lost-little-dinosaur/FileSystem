#include "fs.h"

int my_create(char *filename) {
    // �� Ϊ���ļ�����һ�����д��ļ�������û�п��б����򷵻�-1������ʾ������Ϣ��
    // �� �����ļ��ĸ�Ŀ¼�ļ���û�д򿪣������ my_open()�򿪣�����ʧ�ܣ����ͷŢ���Ϊ�½��ļ�����Ŀ����ļ��򿪱������ - 1������ʾ������Ϣ��
    // �� ���� do_read()�����ø�Ŀ¼�ļ����ݵ��ڴ棬����Ŀ¼�����ļ��Ƿ����������������ͷŢ��з���Ĵ��ļ����������my_close()�رբ��д򿪵�Ŀ¼�ļ���Ȼ�󷵻� - 1������ʾ������Ϣ��
    // �� ��� FAT �Ƿ��п��е��̿飬������Ϊ���ļ�����һ���̿飬�����ͷŢ��з���Ĵ��ļ����������my_close()�رբ��д򿪵�Ŀ¼�ļ������� - 1������ʾ������Ϣ��
    // �� �ڸ�Ŀ¼��Ϊ���ļ�Ѱ��һ�����е�Ŀ¼���Ϊ��׷��һ���µ�Ŀ¼��;���޸ĸ�Ŀ¼�ļ��ĳ�����Ϣ��������Ŀ¼�ļ����û����ļ������е�fcbstate��Ϊ1��
    // �� ׼�������ļ��� FCB �����ݣ��ļ�������Ϊ�����ļ�������Ϊ 0���Ը���д��ʽ����do_write()������д�����з��䵽�Ŀ�Ŀ¼���У�
    // �� Ϊ���ļ���д���з��䵽�Ŀ��д��ļ����fcbstate �ֶ�ֵΪ 0����дָ��ֵΪ0��
    // �� ���� my_close()�رբ��д򿪵ĸ�Ŀ¼�ļ���
    // �� �����ļ��Ĵ��ļ����������Ϊ���ļ����������ء�
    char *fname = strtok(filename, ".");//�ֽ��ַ��� filename Ϊһ���ַ�����"."Ϊ�ָ�����
    char *exname = strtok(NULL, ".");
    if (strcmp(fname, "") == 0) {
        printf("�������ļ���!\n");
        return -1;
    }
    if (!exname) {
        printf("�������׺��!\n");
        return -1;
    }
    if (openfilelist[currfd].metadata == 1) {
        printf("�����ļ��²�����ʹ�� create\n");
        return -1;
    }
    //��ȡ currfd ��Ӧ���ļ�
    openfilelist[currfd].filePtr = 0;
    char buf[MAX_TEXT_SIZE];
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb *fcbPtr = (fcb *) (buf);
    //������û�������ļ�
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (strcmp(fcbPtr->filename, filename) == 0 &&
            strcmp(fcbPtr->exname, exname) == 0) {
            printf("����ͬ���ļ�����!\n");
            return -1;
        }
    }
    //Ѱ�ҿյ� fcb ��
    fcbPtr = (fcb *) (buf);
    fcb *debug = (fcb *) (buf);
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (fcbPtr->free == 0)break;
    }
    //ȡһ���̿�
    int blockNum = getFreeBLOCK();
    if (blockNum == -1) {
        return -1;
    }
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    fat1[blockNum].id = END;
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3);
    memcmp(fat2, fat1, BLOCKSIZE * 2);
    //�� fcb ��д��Ϣ
    strcpy(fcbPtr->filename, filename);
    strcpy(fcbPtr->exname, exname);
    time_t rawtime = time(NULL);
    struct tm *time = localtime(&rawtime);
    fcbPtr->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 +
                   (time->tm_mday);
    fcbPtr->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 +
                   (time->tm_sec) / 2;
    fcbPtr->first = blockNum;
    fcbPtr->free = 1;
    fcbPtr->length = 0;
    fcbPtr->metadata = 1;
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    //�޸ĵ�ǰĿ¼�ļ���.Ŀ¼��ĳ���
    fcbPtr = (fcb *) buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].fcbstate = 1;
}
