#include "fs.h"

void my_rmdir(char *dirname) {
    //����,���ǲ�����ɾ���ǿյ�Ŀ¼�ļ�
    //1. �ѵ�ǰĿ¼��Ŀ¼�ļ���ȡ�� buf ��
    //2. �����������û��ƥ�� dirname ��Ŀ¼�ļ�
    //3. ɾ�����Ŀ¼�ļ�ռ�õ������̿�(Ҳ���ǰ���ռ�õ� fat ȫ���ͷ�,���ұ��ݵ� fat2)
    //4. ���µ�ǰĿ¼�ļ�,�� dirname ��Ӧ�� fcb ���,���Ҹ��µ�ǰĿ¼�ļ��Ĵ�С
    //�� ����\a\Ŀ¼��,ɾ�� b ���Ŀ¼,��ôɾ����֮��,a ���Ŀ¼�ļ��Ĵ�С(length)Ҫ-=sizeof(fcb)
    // ����,b ���Ŀ¼�ļ�����һ�� fcb �� a ���Ŀ¼�ļ����,����� fcb Ҳɾ��
    char *fname = strtok(dirname, ".");
    char *exname = strtok(NULL, ".");
    //������ɾ��.��..����������Ŀ¼�ļ�
    if (strcmp(dirname, ".") == 0 || strcmp(dirname, "..") == 0) {
        printf("����ɾ��%s�������Ŀ¼��\n", dirname);
        return;
    }
    if (exname) {
        printf("ɾ��Ŀ¼�ļ����������׺��!\n");
        return;
    }
    //��ȡ currfd ��Ӧ��Ŀ¼�ļ��� buf
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    int i=0;
    fcb *fcbPtr = (fcb *) buf;
    //Ѱ�ҽ�������ֵ��ļ�Ŀ¼��
    for (i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb));
         i++, fcbPtr++) {
        if (strcmp(fcbPtr->filename, fname) == 0 && fcbPtr->metadata == 0) {
            break;
        }
    }
    if (i == (int) (openfilelist[currfd].length / sizeof(fcb))) {
        printf("û�����Ŀ¼�ļ�\n");
        return;
    }
    //�ж����Ŀ¼�ļ���,�����û��,���ǲ�����ɾ��û����յ�Ŀ¼
    if (fcbPtr->length > 2 * sizeof(fcb)) {
        printf("����������Ŀ¼�µ������ļ�,��ɾ��Ŀ¼�ļ�\n");
        return;
    }
    //������Ŀ¼��ռ�ݵ� FAT
    int blockNum = fcbPtr->first;
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    int next = 0;
    while (1) {
        next = fat1[blockNum].id;
        fat1[blockNum].id = END;
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
    openfilelist[currfd].fcbstate = 1;
}

