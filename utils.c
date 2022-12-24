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
    printf("Command Name\t\t\t Command Parameters\t\t\t Command Functions\n\n");//������\t\t\t �������\t\t\t �����\n\n
    printf("format\t\t\t nothing\t\t\t format\n");//format\t\t\t ��\t\t\t ��ʽ��\n
    printf("cd\t\t\t Directory name (pathname)\t\t\t Switch the current directory to the specified directory\n");//cd\t\t\t Ŀ¼��(·����)\t\t\t �л���ǰĿ¼��ָ��Ŀ¼
    printf("mkdir\t\t\t Directory Name\t\t\t\t Create a new directory in the current directory\n");//mkdir\t\t\t Ŀ¼��\t\t\t\t �ڵ�ǰĿ¼������Ŀ¼\n
    printf("rmdir\t\t\t Directory Name\t\t\t\t Delete the specified directory in the current directory\n");//Ŀ¼��  �ڵ�ǰĿ¼ɾ��ָ��Ŀ¼
    printf("ls\t\t\t nothing\t\t\t\t Display directories and files under the current directory\n");//ls\t\t\t ��\t\t\t\t ��ʾ��ǰĿ¼�µ�Ŀ¼���ļ�\n
    printf("create\t\t\t file name\t\t\t\t Create the specified file in the current directory\n");//create\t\t\t �ļ���\t\t\t\t �ڵ�ǰĿ¼�´���ָ���ļ�
    printf("rm\t\t\t file name\t\t\t\t Delete the specified file in the current directory\n");//rm\t\t\t �ļ���\t\t\t\t �ڵ�ǰĿ¼��ɾ��ָ���ļ�
    printf("open\t\t\t file name\t\t\t\t Open the specified file in the current directory\n");//open\t\t\t �ļ���\t\t\t\t �ڵ�ǰĿ¼�´�ָ���ļ�
    printf("write\t\t\t nothing\t\t\t\t Write the file when the file is open\n");//write\t\t\t ��\t\t\t\t �ڴ��ļ�״̬�£�д���ļ�
    printf("read\t\t\t File name. suffix\t\t\t\t Read the file when it is open\n");//read\t\t\t �ļ���.��׺\t\t\t\t �ڴ��ļ�״̬�£���ȡ���ļ�
    printf("close\t\t\t nothing\t\t\t\t In the open file state, close the file\n");//close\t\t\t ��\t\t\t\t �ڴ��ļ�״̬�£��رո��ļ�
    printf("exit\t\t\t nothing\t\t\t\t Exit the system\n\n");//exit\t\t\t ��\t\t\t\t �˳�ϵͳ
}

void error(char *command) {
    printf("%S: Missing parameter\n", command);//%s : ȱ�ٲ���\n
    printf("Enter 'help' to view the command prompt.\n");//���� 'help' ���鿴������ʾ.
}
