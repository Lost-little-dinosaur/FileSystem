#include "fs.h"

void my_exitsys() {
    //�� currfd=0 ��ʱ��,Ҳ���Ǹ��ڵ�, ���ǲ��ø��µ�
    //��Ϊ����֮ǰ���κ�Ŀ¼�� mkdir ���� create ʱ,�Ѿ��� length �ı仯д��Ŀ¼�ļ���,���ֽ�.��Ŀ¼������
    //���κ�Ŀ¼�ļ���'.'Ŀ¼���ʵʱ���µ�,�������ĸ�Ŀ¼�ļ���û�б�����,������Ҫһ���� close
    //���Ǹ�Ŀ¼û�и�Ŀ¼�ļ�, ���Բ���Ҫ close.
    while (currfd) {
        my_close(currfd);
    }
    FILE *fp = fopen(FILENAME, "w");
    fwrite(v_addr0, SIZE, 1, fp);
    fclose(fp);
}