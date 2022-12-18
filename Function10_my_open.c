#include "fs.h"


int my_open(char *filename) {
    //�ѵ�ǰĿ¼�ļ���ȡ�� buf ��,buf �������һ���� fcb ��,����Щ fcb ��Ѱ��ƥ����filename �������ļ�
    //Ȼ��� openfilelist ��ȡ��һ�����ļ�����,����������ļ�����Ϣд��ȥ,�л�currfd = fd�����Ǵ��ļ���
    //�ѵ�ǰĿ¼�ļ���ȡ�� buf ��
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, openfilelist[currfd].length, buf);
    char *fname = strtok(filename, ".");
    char *exname = strtok(NULL, ".");
    if (!exname) {
        printf("�������׺��\n");
        return -1;
    }
    int i;
    fcb *fcbPtr = (fcb *) buf;
    //Ѱ�Ҵ��ļ�
    for (i = 0; i < (int)(openfilelist[currfd].length / sizeof(fcb));
    i++, fcbPtr++){
        if (strcmp(fcbPtr->filename, fname) == 0 &&
            strcmp(fcbPtr->exname, exname) == 0 && fcbPtr->metadata == 1) {
            break;
        }
    }
    if (i == (int)(openfilelist[currfd].length / sizeof(fcb))){
        printf("�����ڴ��ļ�!\n");
        return -1;
    }
    //Ϊ������һ�����ļ�����
    int fd = get_Free_Openfile();
    if (fd == -1) {
        printf("�û����ļ����Ѿ�����\n");
        return -1;
    }
    openfilelist[fd].metadata = 1;
    openfilelist[fd].filePtr = 0;
    openfilelist[fd].date = fcbPtr->date;
    openfilelist[fd].time = fcbPtr->time;
    strcpy(openfilelist[fd].exname, exname);
    strcpy(openfilelist[fd].filename, fname);
    openfilelist[fd].length = fcbPtr->length;
    openfilelist[fd].first = fcbPtr->first;
//    strcpy(openfilelist[fd].dir, (string(openfilelist[currfd].dir) +
//                                  string(filename)).c_str());
    openfilelist[fd].dirno = openfilelist[currfd].first;
    openfilelist[fd].diroff = i;
    openfilelist[fd].free = 1;
    openfilelist[fd].topenfile = 1;
    openfilelist[fd].fcbstate = 0;
    currfd = fd;
    return 1;
}
