#include "fs.h"


int my_write(int fd) {
    if (fd > MAXOPENFILE || fd < 0) { //��� fd ����Ч�ԣ������Ч�򷵻�-1��
        printf("�˴��ļ�������\n");
        return -1;
    }
    int wstyle;
    while (1) {
        printf("����: 0=�ض�д, 1=����д, 2=׷��д\n");
        scanf("%d", &wstyle);
        if (wstyle > 2 || wstyle < 0) {
            printf("ָ�����!\n");
        } else {
            break;
        }
    }
    char text[MAX_TEXT_SIZE] = "\0";
    char textTmp[MAX_TEXT_SIZE] = "\0";
    char Tmp[MAX_TEXT_SIZE] = "\0";
    char Tmp2[4] = "\0";
    printf("�������ļ�����, �� END Ϊ�ļ���β\n");
    getchar();
    while (fgets(Tmp, 100, stdin)) {
        for (int i = 0; i < strlen(Tmp) - 1; i++) {
            textTmp[i] = Tmp[i];
            textTmp[i + 1] = '\0';
        }
        if (strlen(Tmp) >= 3) {
            Tmp2[0] = Tmp[strlen(Tmp) - 4];
            Tmp2[1] = Tmp[strlen(Tmp) - 3];
            Tmp2[2] = Tmp[strlen(Tmp) - 2];
            Tmp2[3] = '\0';
        }
        if (strcmp(textTmp, "END") == 0 || strcmp(Tmp2, "END") == 0) {
            break;
        }
        textTmp[strlen(textTmp)] = '\n';
        strcat(text, textTmp);
    }
    text[strlen(text)] = '\0';
    //+1 ����ΪҪ�ѽ�β��\0 Ҳд��ȥ
    do_write(fd, text, strlen(text) + 1, wstyle);
    openfilelist[fd].fcbstate = 1;
    return 1;
}


