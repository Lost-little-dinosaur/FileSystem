#include "fs.h"

void my_cd(char *dirname) {
    //�ܽ�:
    // 1. �����ǰ��Ŀ¼�ļ���,��ô��Ҫ�����Ŀ¼�ļ���ȡ�� buf ��, Ȼ���������ļ���� fcb ��û��ƥ�� dirname ��Ŀ¼��(���ұ�����Ŀ¼�ļ�)
    // �����,�Ǿ��� openfilelist ��ȡһ�����ļ�����,�����dirname ���Ŀ¼�ļ��� fcb д��ȥ,Ȼ���л� currfd=fd�����������Ǵ�һ��Ŀ¼��
    if (openfilelist[currfd].metadata == 1) { //�û����ļ�����ļ������ֶΣ�1 ��ʾ�����ļ���0 ��ʾĿ¼�ļ�
        printf("���ļ�����һ��Ŀ¼�ļ�������һ�������ļ����޷�ʹ��cd�������\n");
        return;
    } else {//�����Ŀ¼�ļ�
        //Ѱ��Ŀ¼�ļ�������û��ƥ�������, �Ȱ�Ŀ¼�ļ�����Ϣ��ȡ�� buf ��
        char *buf = (char *) malloc(MAXOPENFILE); //����һ�����������������Ŀ¼�ļ������ݣ���СΪ MAXOPENFILE
        openfilelist[currfd].filePtr = 0; //���ļ�ָ��ָ���ļ���ͷ
        do_read(currfd, openfilelist[currfd].length, buf); //Ŀ¼�ļ���Ϣ����buf��
        int i;
        fcb *fcbPtr = (fcb *) buf; //�� buf �������ǿ��ת���� fcb ����
        for (i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb)); i++) { //����Ŀ¼�ļ�������� fcb
            if (strcmp(fcbPtr->filename, dirname) == 0 && fcbPtr->metadata == 0) { //����ҵ���ƥ���Ŀ¼�ļ�
                break;
            } else {
                fcbPtr++; //ָ����һ�� fcb,��������
            }
        }
        if (fcbPtr->exname[0]!='d' || fcbPtr->exname[1]!='i'){ //������ cd ��Ŀ¼�ļ�
            printf("���ļ�����һ��Ŀ¼�ļ�������һ�������ļ����޷�ʹ��cd�������\n");
            return;
        } else { //��� cd ��һ��Ŀ¼�ļ�, ��ô�ж���.����..�������ļ�,��������ļ�������Ŀ¼�ļ���openfilelist��
            if (strcmp(fcbPtr->filename, ".") == 0) { //cd .�����з�Ӧ
                return;
            } else if (strcmp(fcbPtr->filename, "..") == 0) {//cd ..��Ҫ�ж������ǲ��Ǹ�Ŀ¼, ����Ǹ�Ŀ¼,������, ����,������һ��
                if (currfd == 0) {
                    return;
                } else {
                    currfd = my_close(currfd);
                    return;
                }
            } else { //cd ���ļ�
                int fd = get_Free_Openfile(); //�Ӵ��ļ���������һ�����еĴ��ļ�����
                if (fd == -1) { //���û�п��еĴ��ļ�������ߴ��ļ����Ѿ�����
                    return;
                } else {
                    // �����Ŀ¼�ļ��� fcb ��Ӧ����Ϣд�����ļ�������
                    openfilelist[fd].metadata = fcbPtr->metadata;
                    openfilelist[fd].filePtr = 0;
                    openfilelist[fd].date = fcbPtr->date;
                    openfilelist[fd].time = fcbPtr->time;
                    strcpy(openfilelist[fd].filename,
                           fcbPtr->filename);
                    strcpy(openfilelist[fd].exname, fcbPtr->exname);
                    openfilelist[fd].first = fcbPtr->first;
                    openfilelist[fd].free = fcbPtr->free;
                    openfilelist[fd].length = fcbPtr->length;

                    openfilelist[fd].fcbstate = 0;
                    strcpy(openfilelist[fd].dir,
                           (char *) (strcat(strcat(openfilelist[currfd].dir, dirname), "\\"))); //�ѵ�ǰĿ¼��·��д�����ļ�������
                    openfilelist[fd].topenfile = 1;
                    openfilelist[fd].dirno = openfilelist[currfd].first;
                    openfilelist[fd].diroff = i;
                    currfd = fd; //�л���ǰĿ¼
                }
            }
        }
    }
}