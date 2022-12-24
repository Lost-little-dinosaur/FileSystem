#include "fs.h"


int do_read(int fd, int len, char *text) {
    // ① 使用 malloc()申请 1024B 空间作为缓冲区 buf，申请失败则返回-1，并显示出错信息；
    // ② 将读写指针转化为逻辑块块号及块内偏移量 off，利用打开文件表表项中的首块号查找FAT表，
    // 找到该逻辑块所在的磁盘块块号；将该磁盘块块号转化为虚拟磁盘上的内存位置；
    // ③ 将该内存位置开始的 1024B（一个磁盘块）内容读入 buf 中；
    // ④ 比较 buf 中从偏移量 off 开始的剩余字节数是否大于等于应读写的字节数len，如果是，
    // 则将从off开始的buf中的len长度的内容读入到text中；否则，将从off开始的buf中的剩余内容读入到text中；
    // ⑤ 将读写指针增加④中已读字节数，将应读写的字节数 len 减去④中已读字节数，若len大于0，则转②；否则转⑥；
    // ⑥ 使用 free()释放①中申请的 buf。
    // ⑦ 返回实际读出的字节数。
    // lenTmp 用于记录要求读取的长度,一会返回实际读取的长度
    int lenTmp = len;
    unsigned char *buf = (unsigned char *) malloc(1024);
    if (buf == NULL) {
        printf("do_ Read failed to request memory space\n");//do_read 申请内存空间失败
        return -1;
    }
    int off = openfilelist[fd].filePtr;
    //当前 fd 对应的起始盘块号, 后面变成当前盘块号
    int blockNum = openfilelist[fd].first;
    //fatPtr 当前盘块对应的 fat
    fat *fatPtr = (fat * )(v_addr0 + BLOCKSIZE) + blockNum;
    while (off >= BLOCKSIZE) {
        off -= BLOCKSIZE;
        blockNum = fatPtr->id;
        if (blockNum == END) {
            printf("do_ The block searched by read does not exist\n");//do_read 寻找的块不存在
            return -1;
        }
        fatPtr = (fat * )(v_addr0 + BLOCKSIZE) + blockNum;
    }
    //当前盘块号对应的盘块
    unsigned char *blockPtr = v_addr0 + BLOCKSIZE * blockNum;
    //把文件内容读进 buf
    memcpy(buf, blockPtr, BLOCKSIZE);
    char *textPtr = text;
    while (len > 0) {
        //一个盘块就能放的下
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
            //寻找下一个块
            blockNum = fatPtr->id;
            if (blockNum == END) {
                printf("LEN is too long! File size exceeded!\n");//len 长度过长! 超出了文件大小
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

