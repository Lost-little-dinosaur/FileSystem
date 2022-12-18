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
    printf("������\t\t\t �������\t\t\t �����\n\n");
    printf("format\t\t\t ��\t\t\t ��ʽ��\n");
    printf("cd\t\t\t Ŀ¼��(·����)\t\t\t �л���ǰĿ¼��ָ��Ŀ¼\n");
    printf("mkdir\t\t\t Ŀ¼��\t\t\t\t �ڵ�ǰĿ¼������Ŀ¼\n");
    printf("rmdir\t\t\t Ŀ¼��\t\t\t\t �ڵ�ǰĿ¼ɾ��ָ��Ŀ¼\n");
    printf("ls\t\t\t ��\t\t\t\t ��ʾ��ǰĿ¼�µ�Ŀ¼���ļ�\n");
    printf("create\t\t\t �ļ���\t\t\t\t �ڵ�ǰĿ¼�´���ָ���ļ�\n");
    printf("rm\t\t\t �ļ���\t\t\t\t �ڵ�ǰĿ¼��ɾ��ָ���ļ�\n");
    printf("open\t\t\t �ļ���\t\t\t\t �ڵ�ǰĿ¼�´�ָ���ļ�\n");
    printf("write\t\t\t ��\t\t\t\t �ڴ��ļ�״̬�£�д���ļ�\n");
    printf("read\t\t\t �ļ���.��׺\t\t\t\t �ڴ��ļ�״̬�£���ȡ���ļ�\n");
    printf("close\t\t\t ��\t\t\t\t �ڴ��ļ�״̬�£��رո��ļ�\n");
    printf("exit\t\t\t ��\t\t\t\t �˳�ϵͳ\n\n");
}

void error(char *command) {
    printf("%s : ȱ�ٲ���\n", command);
    printf("���� 'help' ���鿴������ʾ.\n");
}
