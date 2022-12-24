#include "fs.h"


int do_write(int fd, char *text, int len, char wstyle) {
    int blockNum = openfilelist[fd].first;
    fat *fatPtr = (fat *) (v_addr0 + BLOCKSIZE) + blockNum;
    //����д�뷽ʽԤ����
    // 0 �ض�д,ֱ�Ӵ�ͷ��ʼд,ƫ�������� 0 ��,���ҳ��ȱ�� 0
    fat *fat1 =  (fat*) (v_addr0 + BLOCKSIZE);
    if (wstyle == 0) {
        openfilelist[fd].filePtr = 0;
        openfilelist[fd].length = 0;
    }
        //1,����д, ����������ļ�,��ôҪ����ɾ���ļ�ĩβ��\0 ���ܼ�������д
    else if (wstyle == 1) {
        if (openfilelist[fd].metadata == 1 && openfilelist[fd].length != 0) {
            openfilelist[fd].filePtr -= 1;
        }
    }
        //2 ׷��д,�Ͱ��α�ָ��ĩβ
    else if (wstyle == 2) {
        if (openfilelist[fd].metadata == 0) {
            openfilelist[fd].filePtr = openfilelist[fd].length;
        }
            //ͬ��,����������ļ�Ҫɾ��ĩβ��\0
        else if (openfilelist[fd].metadata == 1 && openfilelist[fd].length != 0) {
            openfilelist[fd].filePtr = openfilelist[fd].length - 1;
        }
    }
    int off = openfilelist[fd].filePtr;
    //��� off > BLOCKSIZE,Ҳ�����α�����ָ��Ĳ����ļ��еĵ�һ���̿�,��ô��Ҫ�ҵ��Ǹ��̿�
    //����,����α�ܴ�,������Ѱ�Ҷ�Ӧ�̿��ʱ����û���Ǹ��̿�,��ô��ȱ�ٵ��̿�ȫ������
    while (off >= BLOCKSIZE) {
        blockNum = fatPtr->id;
        if (blockNum == END) {
            blockNum = getFreeBLOCK();
            if (blockNum == END) {
                printf("�̿鲻��\n");
                return -1;
            } else {
                //update FAT
                fatPtr->id = blockNum;
                fatPtr = (fat *) (v_addr0 + BLOCKSIZE + blockNum);
                fatPtr->id = END;
            }
        }
        fatPtr = (fat *) (v_addr0 + BLOCKSIZE) + blockNum;
        off -= BLOCKSIZE;
    }
    unsigned char *buf = (unsigned char *) malloc(BLOCKSIZE *
                                                  sizeof(unsigned char));
    if (buf == NULL) {
        printf("Failed to request memory space!\n");//�����ڴ�ռ�ʧ��!
        return -1;
    }
    fcb *dBlock = (fcb *) (v_addr0 + BLOCKSIZE * blockNum);
    fcb *dFcb = (fcb *) (text);
    unsigned char *blockPtr = (unsigned char *) (v_addr0 + BLOCKSIZE * blockNum); //�̿�ָ��
    int lenTmp = 0;
    char *textPtr = text;
    fcb *dFcbBuf = (fcb *) (buf);
    //�ڶ���ѭ��,��ȡ�̿����ݵ� buf, �� text ����д�� buf, Ȼ���ٴ� buf д�뵽�̿�
    while (len > lenTmp) {
        //�̿����ݶ�ȡ�� buf ��
        memcpy(buf, blockPtr, BLOCKSIZE);
        //�� text ����д�� buf ����ȥ
        for (; off < BLOCKSIZE; off++) {
            *(buf + off) = *textPtr;
            textPtr++;
            lenTmp++;
            if (len == lenTmp) {
                break;
            }
        }
        //�� buf ����д���̿�����ȥ
        memcpy(blockPtr, buf, BLOCKSIZE);
        //��� off==BLCOKSIZE,��ζ�� buf д����, ��� len != lebTmp ��ζ�����ݻ�ûд��, ��ô��Ҫ��������ļ�����û��ʣ���̿�
        //û��ʣ���̿�,�Ǿ�Ҫ�����µ��̿���
        if (off == BLOCKSIZE && len != lenTmp) {
            off = 0;
            blockNum = fatPtr->id;
            if (blockNum == END) {
                blockNum = getFreeBLOCK();
                if (blockNum == END) {
                    printf("Disk is used up!\n");//�̿�������
                    return -1;
                } else {
                    blockPtr = (unsigned char *) (v_addr0 + BLOCKSIZE * blockNum);
                    fatPtr->id = blockNum;
                    fatPtr = (fat *) (v_addr0 + BLOCKSIZE) + blockNum;
                    fatPtr->id = END;
                }
            } else {
                blockPtr = (unsigned char *) (v_addr0 + BLOCKSIZE *
                                                        blockNum);
                fatPtr = (fat *) (v_addr0 + BLOCKSIZE) + blockNum;
            }
        }
    }
    openfilelist[fd].filePtr += len;
    //����дָ�����ԭ���ļ��ĳ��ȣ����޸��ļ��ĳ���
    if (openfilelist[fd].filePtr > openfilelist[fd].length)
        openfilelist[fd].length = openfilelist[fd].filePtr;
    free(buf);
    //���ԭ���ļ�ռ�����̿�,�����޸����ļ�,���ռ�õ��̿������,�Ǿ�Ҫ�Ѻ���ռ�õ��̿�ȫ���ͷŵ�
    int i = blockNum;
    while (1) {
        //������ fat ����һ�� fat ���� end,��ô�����ͷŵ���,һ·�ͷ���ȥ
        if (fat1[i].id != END) {
            int next = fat1[i].id;
            fat1[i].id = FREE;
            i = next;
        } else {
            break;
        }
    }
    //�����������ֲ���,��ѱ��ļ������һ���̿�Ҳ��� free,����Ҫ�����������ó� END
    fat1[blockNum].id = END;
    //update fat2
    memcpy((fat *) (v_addr0 + BLOCKSIZE * 3), (fat *) (v_addr0 + BLOCKSIZE), 2 * BLOCKSIZE);
    return len;
}

