//
// Created by 31447 on 2022/12/26.
//
#include "stdio.h"
#include "string.h"
#include "fs.h"

void my_tips(char *sp) {
    char *p = strtok(sp, "\t");
    // �г���ǰĿ¼�����е��ļ��������浽filenameArr��
    char filenameArr[MAXOPENFILE][MAX_TEXT_SIZE];
    char buf[MAX_TEXT_SIZE];
    openfilelist[currfd].filePtr = 0;
    do_read(currfd, (int) openfilelist[currfd].length, buf);
    fcb *fcbPtr = (fcb *) buf;
    for (int i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb)); i++) { //fcb��СΪ28
        if (fcbPtr->free == 1) {
            if (fcbPtr->metadata == 0) {
                strcpy(filenameArr[i], fcbPtr->filename);
            } else {
                strcpy(filenameArr[i], fcbPtr->filename);
                strcat(filenameArr[i], ".");
                strcat(filenameArr[i], fcbPtr->exname);
            }
        }
        fcbPtr++;
    }
    // �ж�filenameArr��ÿ��Ԫ���Ƿ����cmd[tempI]������
    for (int i = 0; i < (int) (openfilelist[currfd].length / sizeof(fcb)); i++) {
        if (strstr(filenameArr[i], p) != NULL) {
            printf("%s\n", filenameArr[i]);
        }
    }
}