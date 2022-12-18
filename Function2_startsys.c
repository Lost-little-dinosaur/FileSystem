#include "fs.h"


//1. Ѱ�� myfsys.txt �ļ�,������ڶ��ҿ�ͷ���ļ�ħ��,�Ͷ�ȡ��myvhard,���򴴽��ļ���д���ʼ����Ϣ
//2. �����û����ļ���ĵ�һ������, ����Ϊ��Ŀ¼����, Ҳ����Ĭ�ϴ򿪸�Ŀ¼
//3. ��ʼ��һ��ȫ�ֱ���
void startsys() {
    // �� ����������̿ռ䣻16
    // �� ʹ�� c ���ԵĿ⺯�� fopen()�� myfsys �ļ������ļ����ڣ���ת�ۣ����ļ������ڣ��򴴽�֮��ת��
    // �� ʹ�� c ���ԵĿ⺯�� fread()���� myfsys �ļ����ݵ��û��ռ��е�һ���������У����ж��俪ʼ�� 8 ���ֽ������Ƿ�Ϊ��10101010�����ļ�ϵͳħ����������ǣ���ת�ܣ�����ת�ݣ�
    // �� �������������е����ݸ��Ƶ��ڴ��е�������̿ռ��У�ת��
    // �� ����Ļ����ʾ��myfsys �ļ�ϵͳ�����ڣ����ڿ�ʼ�����ļ�ϵͳ����Ϣ�������� my_format()�Ԣ������뵽��������̿ռ���и�ʽ��������ת�ޣ�
    v_addr0 = (unsigned char *) malloc(SIZE); //����������̿ռ�
    //����ļ������ڻ��߿�ͷ�����ļ�ħ��,�����´����ļ�
    printf("��ʼ��ȡ�ļ�...");
    FILE *file;
    if ((file = fopen(FILENAME, "r")) != NULL) {
        fread(buffer, SIZE, 1, file); //���������ļ���ȡ��������
        fclose(file);
        if (memcmp(buffer, "10101010", 8) == 0) {
            memcpy(v_addr0, buffer, SIZE);
            printf("myfsys ��ȡ�ɹ�!\n");
        }
            //���ļ����ǿ�ͷ�����ļ�ħ��
        else {
            printf("myfsys �ļ�ϵͳ�����ڣ����ڿ�ʼ�����ļ�ϵͳ\n");
            my_format();
            memcpy(buffer, v_addr0, SIZE);
        }
    } else {
        printf("myfsys �ļ�ϵͳ�����ڣ����ڿ�ʼ�����ļ�ϵͳ\n");
        my_format();
        memcpy(buffer, v_addr0, SIZE);
    }
    // �� ��ʼ���û����ļ��������� 0 �������Ŀ¼�ļ�ʹ�ã�����д��Ŀ¼�ļ��������Ϣ�����ڸ�Ŀ¼û���ϼ�Ŀ¼��
    // ���Ա����е� dirno �� diroff �ֱ���Ϊ 5����Ŀ¼������ʼ��ţ��� 0������ ptrcurdir ָ��ָ����û����ļ����
    // �� ����ǰĿ¼����Ϊ��Ŀ¼��
    fcb *root;
    root = (fcb * )(v_addr0 + 5 * BLOCKSIZE);
    strcpy(openfilelist[0].filename, root->filename);
    strcpy(openfilelist[0].exname, root->exname);
    openfilelist[0].metadata = root->metadata;
    openfilelist[0].time = root->time;
    openfilelist[0].date = root->date;
    openfilelist[0].first = root->first;
    openfilelist[0].length = root->length;
    openfilelist[0].free = root->free;
    openfilelist[0].dirno = 5;
    openfilelist[0].diroff = 0;
    strcpy(openfilelist[0].dir, "\\root\\");
    openfilelist[0].filePtr = 0;
    openfilelist[0].fcbstate = 0;
    openfilelist[0].topenfile = 1;
    //��ʼ��ȫ�ֱ���
    //startp ָ���������Ŀ�ͷ
    startp = ((block0 *) v_addr0)->startBlock;
    currfd = 0;
    return;
}