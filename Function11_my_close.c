#include "fs.h"

int my_close(int fd) {
    if (fd > MAXOPENFILE || fd < 0) { //��� fd ����Ч�ԣ������Ч�򷵻�-1��
        printf("�˴��ļ�������\n");
        return -1;
    } else {
        //�жϸ�Ŀ¼�ļ��Ƿ����, ������-1
        int fatherFd = find_father_dir(fd);
        if (fatherFd == -1) {
            printf("��Ŀ¼������");
            return -1;
        }
        //���fcbstate�ֶ�,����� 1,����Ҫ�����ļ���FCB�����ݱ��浽��������ϸ��ļ���Ŀ¼��
        if (openfilelist[fd].fcbstate == 1) {
            char buf[MAX_TEXT_SIZE];
            do_read(fatherFd, openfilelist[fatherFd].length, buf); //�Ѹ�Ŀ¼�ļ��Ӵ����ж�ȡ�� buf ��
            //���� fcb ����
            fcb *fcbPtr = (fcb *) (buf + sizeof(fcb) * openfilelist[fd].diroff); //fcbPtr ָ������ļ��Ķ�Ӧ fcb ��λ��
            //diroff ��Ӧ���ļ���Ŀ¼���ڸ�Ŀ¼�ļ��� dirno �̿��е�Ŀ¼�����
            strcpy(fcbPtr->exname, openfilelist[fd].exname); // �ļ���չ��
            strcpy(fcbPtr->filename, openfilelist[fd].filename); // �ļ���
            fcbPtr->first = openfilelist[fd].first; // �ļ���ʼ�̿��
            fcbPtr->free = openfilelist[fd].free; // ��ʾĿ¼���Ƿ�Ϊ�գ���ֵΪ 0����ʾ�գ�ֵΪ 1����ʾ�ѷ���
            fcbPtr->length = openfilelist[fd].length; // �ļ����ȣ��������ļ����ֽ�������Ŀ¼�ļ�������Ŀ¼�������
            openfilelist[fatherFd].filePtr = 0; // ��дָ�����ļ��е�λ��
            fcbPtr->time = openfilelist[fd].time; // �ļ�����ʱ��
            fcbPtr->date = openfilelist[fd].date; // �ļ���������
            fcbPtr->metadata = openfilelist[fd].metadata; // �ļ������ֶ� 1 ��ʾ�����ļ���0 ��ʾĿ¼�ļ�
            openfilelist[fatherFd].filePtr = openfilelist[fd].diroff * sizeof(fcb);
            do_write(fatherFd, (char *) fcbPtr, sizeof(fcb), 1);
        }
        memset(&openfilelist[fd], 0, sizeof(useropen));
        currfd = fatherFd;
        return fatherFd;
    }
}

