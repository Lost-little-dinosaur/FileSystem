#include "fs.h"


int do_read(int fd, int len, char *text) {
    // �� ʹ�� malloc()���� 1024B �ռ���Ϊ������ buf������ʧ���򷵻�-1������ʾ������Ϣ��
    // �� ����дָ��ת��Ϊ�߼����ż�����ƫ���� off�����ô��ļ�������е��׿�Ų���FAT��
    // �ҵ����߼������ڵĴ��̿��ţ����ô��̿���ת��Ϊ��������ϵ��ڴ�λ�ã�
    // �� �����ڴ�λ�ÿ�ʼ�� 1024B��һ�����̿飩���ݶ��� buf �У�
    // �� �Ƚ� buf �д�ƫ���� off ��ʼ��ʣ���ֽ����Ƿ���ڵ���Ӧ��д���ֽ���len������ǣ�
    // �򽫴�off��ʼ��buf�е�len���ȵ����ݶ��뵽text�У����򣬽���off��ʼ��buf�е�ʣ�����ݶ��뵽text�У�
    // �� ����дָ�����Ӣ����Ѷ��ֽ�������Ӧ��д���ֽ��� len ��ȥ�����Ѷ��ֽ�������len����0����ת�ڣ�����ת�ޣ�
    // �� ʹ�� free()�ͷŢ�������� buf��
    // �� ����ʵ�ʶ������ֽ�����
    // lenTmp ���ڼ�¼Ҫ���ȡ�ĳ���,һ�᷵��ʵ�ʶ�ȡ�ĳ���
    int lenTmp = len;
    unsigned char *buf = (unsigned char *) malloc(1024);
    if (buf == NULL) {
        printf("do_ Read failed to request memory space\n");//do_read �����ڴ�ռ�ʧ��
        return -1;
    }
    int off = openfilelist[fd].filePtr;
    //��ǰ fd ��Ӧ����ʼ�̿��, �����ɵ�ǰ�̿��
    int blockNum = openfilelist[fd].first;
    //fatPtr ��ǰ�̿��Ӧ�� fat
    fat *fatPtr = (fat * )(v_addr0 + BLOCKSIZE) + blockNum;
    while (off >= BLOCKSIZE) {
        off -= BLOCKSIZE;
        blockNum = fatPtr->id;
        if (blockNum == END) {
            printf("do_ The block searched by read does not exist\n");//do_read Ѱ�ҵĿ鲻����
            return -1;
        }
        fatPtr = (fat * )(v_addr0 + BLOCKSIZE) + blockNum;
    }
    //��ǰ�̿�Ŷ�Ӧ���̿�
    unsigned char *blockPtr = v_addr0 + BLOCKSIZE * blockNum;
    //���ļ����ݶ��� buf
    memcpy(buf, blockPtr, BLOCKSIZE);
    char *textPtr = text;
    while (len > 0) {
        //һ���̿���ܷŵ���
        if (BLOCKSIZE - off > len) {
            memcpy(textPtr, buf + off, len);
            textPtr += len;
            off += len;
            openfilelist[fd].filePtr += len;
            len = 0;
        } else {
            memcpy(textPtr, buf + off, BLOCKSIZE - off);
            textPtr += BLOCKSIZE - off;
            off = 0;
            len -= BLOCKSIZE - off;
            //Ѱ����һ����
            blockNum = fatPtr->id;
            if (blockNum == END) {
                printf("LEN is too long! File size exceeded!\n");//len ���ȹ���! �������ļ���С
                break;
            }
            fatPtr = (fat * )(v_addr0 + BLOCKSIZE) + blockNum;
            blockPtr = v_addr0 + BLOCKSIZE * blockNum;
            memcpy(buf, blockPtr, BLOCKSIZE);
        }
    }
    free(buf);
    return lenTmp - len;
}

