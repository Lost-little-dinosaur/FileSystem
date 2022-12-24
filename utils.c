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
    printf("Command Name\t\t\t Command Parameters\t\t\t Command Functions\n\n");//命令名\t\t\t 命令参数\t\t\t 命令功能\n\n
    printf("format\t\t\t nothing\t\t\t format\n");//format\t\t\t 无\t\t\t 格式化\n
    printf("cd\t\t\t Directory name (pathname)\t\t\t Switch the current directory to the specified directory\n");//cd\t\t\t 目录名(路径名)\t\t\t 切换当前目录到指定目录
    printf("mkdir\t\t\t Directory Name\t\t\t\t Create a new directory in the current directory\n");//mkdir\t\t\t 目录名\t\t\t\t 在当前目录创建新目录\n
    printf("rmdir\t\t\t Directory Name\t\t\t\t Delete the specified directory in the current directory\n");//目录名  在当前目录删除指定目录
    printf("ls\t\t\t nothing\t\t\t\t Display directories and files under the current directory\n");//ls\t\t\t 无\t\t\t\t 显示当前目录下的目录和文件\n
    printf("create\t\t\t file name\t\t\t\t Create the specified file in the current directory\n");//create\t\t\t 文件名\t\t\t\t 在当前目录下创建指定文件
    printf("rm\t\t\t file name\t\t\t\t Delete the specified file in the current directory\n");//rm\t\t\t 文件名\t\t\t\t 在当前目录下删除指定文件
    printf("open\t\t\t file name\t\t\t\t Open the specified file in the current directory\n");//open\t\t\t 文件名\t\t\t\t 在当前目录下打开指定文件
    printf("write\t\t\t nothing\t\t\t\t Write the file when the file is open\n");//write\t\t\t 无\t\t\t\t 在打开文件状态下，写该文件
    printf("read\t\t\t File name. suffix\t\t\t\t Read the file when it is open\n");//read\t\t\t 文件名.后缀\t\t\t\t 在打开文件状态下，读取该文件
    printf("close\t\t\t nothing\t\t\t\t In the open file state, close the file\n");//close\t\t\t 无\t\t\t\t 在打开文件状态下，关闭该文件
    printf("exit\t\t\t nothing\t\t\t\t Exit the system\n\n");//exit\t\t\t 无\t\t\t\t 退出系统
}

void error(char *command) {
    printf("%S: Missing parameter\n", command);//%s : 缺少参数\n
    printf("Enter 'help' to view the command prompt.\n");//输入 'help' 来查看命令提示.
}
