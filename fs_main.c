#include "fs.h"


int main() {
    // ��ֹ����̨�������
    system("chcp 65001");

    char cmd[15][10] = {"mkdir", "rmdir", "ls", "cd", "create", "rm", "open",
                        "close", "write", "read", "exit", "help", "format"};
    char temp[30], command[30], *sp, *len;
    int indexOfCmd, i;
    int length = 0;
    startsys();
    printf("�ļ�ϵͳ�ѿ���.\n\n");
    printf("���� help ����ʾ����ҳ��.\n\n");
    while (1) {
        printf("%s>", openfilelist[currfd].dir);
        fgets(temp, 100, stdin);
        indexOfCmd = -1;
        for (int i = 0; i < strlen(temp) - 1; i++) {
            command[i] = temp[i]; //fgets ����һ�� bug�����\nҲ����ȥ������Ҫ΢��һ��
            command[i + 1] = '\0';
        }
        if (strcmp(command, "")) { // ���ǿ�����
            sp = strtok(command, " "); // �ѿո�ǰ������������
            //printf("%s\n",sp);
            for (i = 0; i < 15; i++) {
                if (strcmp(sp, cmd[i]) == 0) {
                    indexOfCmd = i;
                    break;
                }
            }
            switch (indexOfCmd) {
                case 0: // mkdir
                    sp = strtok(NULL, " ");
                    //printf("%s\n",sp);
                    if (sp != NULL)
                        my_mkdir(sp);
                    else
                        error("mkdir");
                    break;
                case 1: // rmdir
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_rmdir(sp);
                    else
                        error("rmdir");
                    break;
                case 2: // ls
                    my_ls();
                    break;
                case 3: // cd
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_cd(sp);
                    else
                        error("cd");
                    break;
                case 4: // create
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_create(sp);
                    else
                        error("create");
                    break;
                case 5: // rm
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_rm(sp);
                    else
                        error("rm");
                    break;
                case 6: // open
                    sp = strtok(NULL, " ");
                    if (sp != NULL)
                        my_open(sp);
                    else
                        error("open");
                    break;
                case 7: // close
                    if (openfilelist[currfd].metadata == 1)
                        my_close(currfd);
                    else
                        printf("��ǰû�еĴ򿪵��ļ�\n");
                    break;
                case 8: // write
                    if (openfilelist[currfd].metadata == 1)
                        my_write(currfd);
                    else
                        printf("���ȴ��ļ�,Ȼ����ʹ�� write ����\n");
                    break;
                case 9: // read
                    sp = strtok(NULL, " ");
                    length = 0;
                    if (sp != NULL) {
                        for (int i = 0; i < strlen(sp); i++)
                            length = length * 10 + sp[i] - '0';
                    }
                    if (length == 0)
                        error("read");
                    else if (openfilelist[currfd].metadata == 1)
                        my_read(currfd, length);
                    else
                        printf("���ȴ��ļ�,Ȼ����ʹ�� read����\n");
                    break;
                case 10: // exit
                    my_exitsys();
                    printf("�˳��ļ�ϵͳ.\n");
                    return 0;
                    break;
                case 11: // help
                    show_help();
                    break;
                case 12:
                    my_format();
                    break;
                default:
                    printf("û�� %s �������\n", sp);
                    break;
            }
        } else
            printf("\n");
    }
    return 0;
}

