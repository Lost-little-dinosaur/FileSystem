#include "fs.h"


int main() {
    // 防止控制台输出乱码
    memset(openfilelist, 0, sizeof(useropen) * MAXOPENFILE);

//    memset(block0, 0, sizeof(block0));
    system("chcp 65001");

    char cmd[15][10] = {"mkdir", "rmdir", "ls", "cd", "create", "rm", "open",
                        "close", "write", "read", "exit", "help", "format", "\n"};
    char temp[30], command[30], *sp, *len;
    int indexOfCmd;
    int length = 0;
    startsys();
    printf("File system is turned on.\n\n");
    printf("Type help to display the help page.\n\n");
    while (1) {
        printf("%s>", openfilelist[currfd].dir);
        fgets(temp, 100, stdin);
        indexOfCmd = -1;
        if (temp[0] == '\n') {  // 创新点：输入回车键，不执行任何操作
            continue;
        }
        for (int i = 0; i < strlen(temp) - 1; i++) {
            command[i] = temp[i]; //fgets 存在一个 bug，会把\n也读进去，所以要微调一下
            command[i + 1] = '\0';
        }
        if (strcmp(command, "") != 0) { // 不是空命令
            sp = strtok(command, " "); // 把空格前的命令分离出来
            //printf("%s\n",sp);
            for (int i = 0; i < 14; i++) {
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
                    if (sp != NULL) {
                        // 判断sp中是否有’/‘
                        len = strchr(sp, '/');
                        if (len != NULL) { // 有’/‘，则按照’/‘分割
                            char *p = strtok(sp, "/");
                            while (p != NULL) {
                                my_cd(p);
                                p = strtok(NULL, "/");
                            }
                        } else { // 没有’/‘，直接cd
                            // 如果sp其中有\t，则进入my_tips()函数
                            if (strchr(sp, '\t') != NULL) {
                                my_tips(sp);
                                continue;
                            }
                            my_cd(sp);
                        }
                        // mkdir k
                        // cd k
                        // mkdir kk
                        // cd ..
                        // cd k/kk
                    } else
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
                        printf("Currently no open files\n");//当前没有的打开的文件
                    break;
                case 8: // write
                    if (openfilelist[currfd].metadata == 1)
                        my_write(currfd);
                    else
                        printf("Please open the file before using the write operation\n");//请先打开文件,然后再使用 write 操作
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
                        printf("Please open the file before using the read operation\n");//请先打开文件,然后再使用 read操作
                    break;
                case 10: // exit
                    my_exitsys();
                    printf("Exit file system.\n");//退出文件系统
                    return 0;
                    break;
                case 11: // help
                    show_help();
                    break;
                case 12:
                    my_format();
                    break;
                case 13:
                    break;
                default:
                    printf("%s: command not found\n", sp);//没有 %s 这个命令
                    break;
            }
        } else
            printf("\n");
    }
    return 0;
}

