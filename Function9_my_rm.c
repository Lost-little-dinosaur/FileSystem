#include "fs.h"

void my_rm(char *filename) {
    //�� ������\a\Ŀ¼��
    //1. ��ȡ��ǰĿ¼��Ŀ¼�ļ�(a)�� buf ��(buf ����һ���� fcb)
    //2. �� buf ��Ѱ��ƥ�� filename �������ļ�
    //3. �������ļ�ռ�ݵ��̿�(Ҳ�����ͷ���ռ�ݵ� fat,���ұ���)
    //4. �ڸ�Ŀ¼�ļ���,ɾ�� filename ��Ӧ�� fcb (a ��ɾ�� b �� fcb)
    //5. ���¸�Ŀ¼�ļ��ĳ���(a ��Ŀ¼�ļ�����)
    char *fname = strtok(filename, ".");
    char *exname = strtok(NULL, ".");
    if (!exname) {
        printf("�������׺��!\n");
        return;
    }
    if (strcmp(exname, "di") == 0) {
        printf("����ɾ��Ŀ¼��\n");
        return;
    }
    //��ȡ currfd ��Ӧ��Ŀ¼�ļ��� buf
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    int i;
    fcb *fcbPtr = (fcb *) buf;
    //Ѱ�ҽ�������ֵ��ļ�Ŀ¼��
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (strcmp(fcbPtr->filename, fname) == 0 && strcmp(fcbPtr->exname, exname) == 0) {
            break;
        }
    }
    if (i == (int)(openfilelist[currfd].length / sizeof(fcb))){
        printf("û������ļ�\n");
        return;
    }
    //������Ŀ¼��ռ�ݵ� FAT
    int blockNum = fcbPtr->first;
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    int next = 0;
    while (1) {
        next = fat1[blockNum].id;
        fat1[blockNum].id = FREE;
        if (next != END) {
            blockNum = next;
        } else {
            break;
        }
    }
    //���� fat2
    fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3);
    memcpy(fat2, fat1, sizeof(fat));
    //�޸���� fcb Ϊ��
    fcbPtr->date = 0;
    fcbPtr->time = 0;
    fcbPtr->exname[0] = '\0';
    fcbPtr->filename[0] = '\0';
    fcbPtr->first = 0;
    fcbPtr->free = 0;
    fcbPtr->length = 0;
    //д��������ȥ, ���� fcb ����Ϊ��
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].length -= sizeof(fcb);
    //����.Ŀ¼��ĳ���
    fcbPtr = (fcb *) buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].length -= sizeof(fcb);
    //����.Ŀ¼��ĳ���
    fcbPtr = (fcb *) buf;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].fcbstate = 1;
}
