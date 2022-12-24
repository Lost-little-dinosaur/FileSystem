#include "fs.h"

void my_mkdir(char *dirname) {
    // 1. �ж� dirname �Ƿ�Ϸ�
    // 2. �򿪵�ǰĿ¼��Ŀ¼�ļ�,�����Ƿ�����
    // 3. ռ��һ���̿�, �� FAT ��Ҫд������̿鱻ռ����
    // 4. ռ��һ�����ļ�����,������Ϊһ��Ҫ�����Ŀ¼�ļ�����д��Ĭ�ϵ�.��..���������ļ�Ŀ¼��
    // ������ do_write,����Ҫ�ȴ�����ļ���.
    // д��.��..�����ļ�Ŀ¼��½���Ŀ¼�ļ���
    // 5. ������ openfilelist
    // 6. �޸ĸ�Ŀ¼�ļ��Ĵ�С,��Ϊ�����һ���ļ�Ŀ¼��
    // �� ��\a\���Ŀ¼�´����� b ���Ŀ¼, ��ô a ���Ŀ¼�ļ��Ĵ�СҪ+=sizeof(fcb)
    //�ж� dirname �Ƿ�Ϸ�
    char *fname = strtok(dirname, ".");
    char *exname = strtok(NULL, ".");
    if (exname) {
        printf("Suffix name is not allowed!\n");//�����������׺��
        return;
    }
    char text[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    int fileLen = do_read(currfd, openfilelist[currfd].length, text);
    //text ������ݾ���һ���� fcb
    fcb *fcbPtr = (fcb *) text;
    for (int i = 0; i < (int) (fileLen / sizeof(fcb)); i++) {
        if (strcmp(dirname, fcbPtr[i].filename) == 0 && fcbPtr->metadata ==
                                                        0) {
            printf("Directory name already exists!\n");//Ŀ¼���Ѿ�����
            return;
        }
    }
    //�ڴ��ļ�������һ�����ļ�����
    int fd = get_Free_Openfile();
    if (fd == -1) {
        printf("All open file tables are occupied\n");//���ļ�����ȫ����ռ��
        return;
    }
    //�� FAT ����һ�����̿�
    unsigned short int blockNum = getFreeBLOCK();
    if (blockNum == END) {
        printf("The disk has been used up\n");//�̿��Ѿ�����
        openfilelist[fd].topenfile = 0;
        return;
    }
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3);
    fat1[blockNum].id = END;
    fat2[blockNum].id = END;
    //�ڵ�ǰĿ¼�������һ������Ҫ��Ŀ¼��
    int i = 0;
    for (; i < (int) (fileLen / sizeof(fcb)); i++) {
        if (fcbPtr[i].free == 0) {
            break;
        }
    }
    openfilelist[currfd].filePtr = i * sizeof(fcb);
    //�޸��� fcb,fcbstate �� 1
    openfilelist[currfd].fcbstate = 1;
    //�޸��½���Ŀ¼��,�� fcb ����
    //��Ϊ��������ģ���ļ�ϵͳ,����Ҫ��д����ʱ�� fcb ��,Ȼ���� do_write תд��������
    fcb *fcbtmp = (fcb *) malloc(sizeof(fcb));
    fcbtmp->metadata = 0;
    time_t rawtime = time(NULL);
    struct tm *time = localtime(&rawtime);
    fcbtmp->date = (time->tm_year - 100) * 512 + (time->tm_mon + 1) * 32 +
                   (time->tm_mday);
    fcbtmp->time = (time->tm_hour) * 2048 + (time->tm_min) * 32 + (time->tm_sec)
                                                                  / 2;
    strcpy(fcbtmp->filename, dirname);
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = blockNum;
    fcbtmp->length = 2 * sizeof(fcb);
    fcbtmp->free = 1;
    //�� do_write �� fcbtmp д�뵽Ŀ¼�ļ���
    do_write(currfd, (char *) fcbtmp, sizeof(fcb), 1);
    //���ô��ļ�����
    openfilelist[fd].metadata = 0;
    openfilelist[fd].filePtr = 0;
    openfilelist[fd].date = fcbtmp->date;
    openfilelist[fd].time = fcbtmp->time;
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    strcpy(openfilelist[fd].exname, "di");
    strcpy(openfilelist[fd].filename, dirname);
    openfilelist[fd].fcbstate = 0;
    openfilelist[fd].first = fcbtmp->first;
    openfilelist[fd].free = fcbtmp->free;
    openfilelist[fd].length = fcbtmp->length;
    openfilelist[fd].topenfile = 1;
    //�ڶ�Ӧ���̿���������������Ŀ¼.��..
    //ͬ������Ҳ����ֱ��д�� fcbPtr ��,ֻ���� fcbTmp д���ڴ���,Ȼ���� do_writeתд���ļ�
    fcbtmp->metadata = 0;
    fcbtmp->date = fcbtmp->date;
    fcbtmp->time = fcbtmp->time;
    strcpy(fcbtmp->filename, ".");
    strcpy(fcbtmp->exname, "di");
    fcbtmp->first = blockNum;
    fcbtmp->length = 2 * sizeof(fcb);
    do_write(fd, (char *) fcbtmp, sizeof(fcb), 1);
    //����..Ŀ¼
    fcb *fcbtmp2 = (fcb *) malloc(sizeof(fcb));
    memcpy(fcbtmp2, fcbtmp, sizeof(fcb));
    strcpy(fcbtmp2->filename, "..");
    fcbtmp2->first = openfilelist[currfd].first;
    fcbtmp2->length = openfilelist[currfd].length;
    fcbtmp2->date = openfilelist[currfd].date;
    fcbtmp2->time = openfilelist[currfd].time;
    do_write(fd, (char *) fcbtmp2, sizeof(fcb), 1);
    my_close(fd);
    //���±� currfd Ŀ¼�ļ��� fcb
    fcbPtr = (fcb *) text;
    fcbPtr->length = openfilelist[currfd].length;
    openfilelist[currfd].filePtr = 0;
    do_write(currfd, (char *) fcbPtr, sizeof(fcb), 1);
    openfilelist[currfd].fcbstate = 1;
    free(fcbtmp);
    free(fcbtmp2);
}