#include "fs.h"


int do_write(int fd, char *text, int len, char wstyle) {
    int blockNum = openfilelist[fd].first;
    fat *fatPtr = (fat *) (v_addr0 + BLOCKSIZE) + blockNum;
    //三种写入方式预处理
    // 0 截断写,直接从头开始写,偏移量就是 0 了,而且长度变成 0
    fat *fat1 =  (v_addr0 + BLOCKSIZE);
    if (wstyle == 0) {
        openfilelist[fd].filePtr = 0;
        openfilelist[fd].length = 0;
    }
        //1,覆盖写, 如果是数据文件,那么要考虑删除文件末尾的\0 才能继续往下写
    else if (wstyle == 1) {
        if (openfilelist[fd].metadata == 1 && openfilelist[fd].length != 0) {
            openfilelist[fd].filePtr -= 1;
        }
    }
        //2 追加写,就把游标指向末尾
    else if (wstyle == 2) {
        if (openfilelist[fd].metadata == 0) {
            openfilelist[fd].filePtr = openfilelist[fd].length;
        }
            //同理,如果是数据文件要删除末尾的\0
        else if (openfilelist[fd].metadata == 1 && openfilelist[fd].length != 0) {
            openfilelist[fd].filePtr = openfilelist[fd].length - 1;
        }
    }
    int off = openfilelist[fd].filePtr;
    //如果 off > BLOCKSIZE,也就是游标现在指向的不是文件中的第一个盘块,那么需要找到那个盘块
    //另外,如果游标很大,但是在寻找对应盘块的时候发现没有那个盘块,那么把缺少的盘块全都补上
    while (off >= BLOCKSIZE) {
        blockNum = fatPtr->id;
        if (blockNum == END) {
            blockNum = getFreeBLOCK();
            if (blockNum == END) {
                printf("盘块不足\n");
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
        printf("申请内存空间失败!\n");
        return -1;
    }
    fcb *dBlock = (fcb *) (v_addr0 + BLOCKSIZE * blockNum);
    fcb *dFcb = (fcb *) (text);
    unsigned char *blockPtr = (unsigned char *) (v_addr0 + BLOCKSIZE * blockNum); //盘块指针
    int lenTmp = 0;
    char *textPtr = text;
    fcb *dFcbBuf = (fcb *) (buf);
    //第二个循环,读取盘块内容到 buf, 把 text 内容写入 buf, 然后再从 buf 写入到盘块
    while (len > lenTmp) {
        //盘块内容读取到 buf 里
        memcpy(buf, blockPtr, BLOCKSIZE);
        //把 text 内容写到 buf 里面去
        for (; off < BLOCKSIZE; off++) {
            *(buf + off) = *textPtr;
            textPtr++;
            lenTmp++;
            if (len == lenTmp) {
                break;
            }
        }
        //把 buf 内容写到盘块里面去
        memcpy(blockPtr, buf, BLOCKSIZE);
        //如果 off==BLCOKSIZE,意味着 buf 写满了, 如果 len != lebTmp 意味着数据还没写完, 那么就要看看这个文件还有没有剩余盘块
        //没有剩余盘块,那就要分配新的盘块了
        if (off == BLOCKSIZE && len != lenTmp) {
            off = 0;
            blockNum = fatPtr->id;
            if (blockNum == END) {
                blockNum = getFreeBLOCK();
                if (blockNum == END) {
                    printf("盘块用完了!\n");
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
    //若读写指针大于原来文件的长度，则修改文件的长度
    if (openfilelist[fd].filePtr > openfilelist[fd].length)
        openfilelist[fd].length = openfilelist[fd].filePtr;
    free(buf);
    //如果原来文件占几个盘块,现在修改了文件,结果占用的盘块变少了,那就要把后面占用的盘块全部释放掉
    int i = blockNum;
    while (1) {
        //如果这个 fat 的下一个 fat 不是 end,那么就是释放掉它,一路释放下去
        if (fat1[i].id != END) {
            int next = fat1[i].id;
            fat1[i].id = FREE;
            i = next;
        } else {
            break;
        }
    }
    //按照上面这种操作,会把本文件的最后一个盘块也变成 free,这里要把他重新设置成 END
    fat1[blockNum].id = END;
    //update fat2
    memcpy((fat *) (v_addr0 + BLOCKSIZE * 3), (fat *) (v_addr0 + BLOCKSIZE), 2 * BLOCKSIZE);
    return len;
}

