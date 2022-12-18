#include "fs.h"

unsigned short int getFreeBLOCK() {
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE);
    for (int i = 0; i < (int) (SIZE / BLOCKSIZE); i++) {
        if (fat1[i].id == FREE) {
            return i;
        }
    }
    return END;
}

int get_Free_Openfile() {
    for (int i = 0; i < MAXOPENFILE; i++) {
        if (openfilelist[i].topenfile == 0) {
            openfilelist[i].topenfile = 1;
            return i;
        }
    }
    return -1;
}

int find_father_dir(int fd) {
    for (int i = 0; i < MAXOPENFILE; i++) {
        if (openfilelist[i].first == openfilelist[fd].dirno) {
            return i;
        }
    }
    return -1;
}

void show_help() {
    printf("命令名\t\t\t 命令参数\t\t\t 命令功能\n\n");
    printf("format\t\t\t 无\t\t\t 格式化\n");
    printf("cd\t\t\t 目录名(路径名)\t\t\t 切换当前目录到指定目录\n");
    printf("mkdir\t\t\t 目录名\t\t\t\t 在当前目录创建新目录\n");
    printf("rmdir\t\t\t 目录名\t\t\t\t 在当前目录删除指定目录\n");
    printf("ls\t\t\t 无\t\t\t\t 显示当前目录下的目录和文件\n");
    printf("create\t\t\t 文件名\t\t\t\t 在当前目录下创建指定文件\n");
    printf("rm\t\t\t 文件名\t\t\t\t 在当前目录下删除指定文件\n");
    printf("open\t\t\t 文件名\t\t\t\t 在当前目录下打开指定文件\n");
    printf("write\t\t\t 无\t\t\t\t 在打开文件状态下，写该文件\n");
    printf("read\t\t\t 文件名.后缀\t\t\t\t 在打开文件状态下，读取该文件\n");
    printf("close\t\t\t 无\t\t\t\t 在打开文件状态下，关闭该文件\n");
    printf("exit\t\t\t 无\t\t\t\t 退出系统\n\n");
}

void error(char *command) {
    printf("%s : 缺少参数\n", command);
    printf("输入 'help' 来查看命令提示.\n");
}
