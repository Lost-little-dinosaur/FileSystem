#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#define BLOCKSIZE 1024 //���̿��С
#define END 65535
#define SIZE 1024000 //������̿ռ��С
#define FREE 0
#define ROOTBLOCKNUM 2  //��Ŀ¼����ռ�̿�����
#define MAXOPENFILE 10  //���ͬʱ���ļ���
#define MAX_TEXT_SIZE  10000        //������������ļ���С
#define FILENAME  "FileSys.txt"     // �ļ�ϵͳ��

typedef struct FCB {
    char filename[8]; //�ļ���
    char exname[4]; //�ļ���չ��
    unsigned char metadata; // �ļ������ֶ� 1 ��ʾ�����ļ���0 ��ʾĿ¼�ļ�
    unsigned char attribute;
    //�ļ������ֶΣ�0��ʾĿ¼�ļ���1��ʾ�����ļ�
    unsigned short time; //�ļ�����ʱ��
    unsigned short date; //�ļ���������
    unsigned short first;
    //�ļ���ʼ�̿��
    unsigned long length;
    //�ļ�����(�ֽ���)
    char free;
    //��ʾĿ¼���Ƿ�Ϊ�գ�0��ʾ�գ�1��ʾ�ѷ���
} fcb;


typedef struct FAT {
    unsigned short id;
} fat;

typedef struct USEROPEN {
    char filename[8];
    //�ļ���(���Դ�·����openfilelist[1]���ǵ�ǰĿ¼������������·��)
    char exname[3]; //�ļ���չ��
    unsigned char attribute;
    //�ļ������ֶΣ�0��ʾĿ¼�ļ���1��ʾ�����ļ�
    unsigned short time; //�ļ�����ʱ��
    unsigned short date; //�ļ���������
    unsigned short first;
    //�ļ���ʼ�̿��
    unsigned long length;
    char dir[80];
    int count;
    //�ļ�����(�ֽ���)
    char free;
    unsigned char metadata; // �ļ������ֶ� 1 ��ʾ�����ļ���0 ��ʾĿ¼�ļ�
    //��ʾĿ¼���Ƿ�Ϊ�գ�0��ʾ�գ�1��ʾ�ѷ���
    int dirno;
    //��Ӧ���ļ���Ŀ¼���ڸ�Ŀ¼�ļ��е��̿��
    int diroff;
    //��Ӧ���ļ���Ŀ¼���ڸ�Ŀ¼�ļ���dirno�̿��е�Ŀ¼�����
    char fcbstate;
    //�Ƿ��޸����ļ���FCB���ݣ�����޸�����Ϊ1������Ϊ0
    char topenfile;
    //��ʾ���û��򿪱����Ƿ�Ϊ�գ���ֵΪ0����ʾΪ�գ������ʾ�ѱ�ĳ�ļ�ռ��
    int filePtr;
    // ��дָ�����ļ��е�λ��
} useropen;

typedef struct BLOCK0 {//�����������
    char magic_number[8]; // �ļ�ϵͳ��ħ��
    char information[200]; //������Ϣ
    unsigned short root; //rootָ��ָ���ļ�ϵͳ�ĸ�Ŀ¼
    unsigned char *startBlock; //startBlockָ��ָ���ļ�ϵͳ�е�һ�����õĴ��̿�
} block0;


void startsys(); // �����ļ�ϵͳ
void my_format(); // ���̸�ʽ������
void my_cd(char *dirname); // ���ڸ��ĵ�ǰĿ¼
void my_mkdir(char *dirname); // ������Ŀ¼
void my_rmdir(char *dirname); // ɾ����Ŀ¼
void my_ls(); // ��ʾĿ¼�е�����
void my_create(char *filename); // �����ļ�
void my_rm(char *filename); // ɾ���ļ�
int my_open(char *filename); // ���ļ�
int my_close(int fd); // �ر��ļ�
int my_write(int fd); // д�ļ�
int do_write(int fd, char *text, int len, char wstyle);// ʵ��д�ļ�
int my_read(int fd, int len); // ���ļ�
int do_read(int fd, int len, char *text); // ʵ�ʶ��ļ�
void my_exitsys(); // �˳��ļ�ϵͳ
unsigned short getFreeBLOCK(); // ��ȡһ�����еĴ��̿�
int get_Free_Openfile(); // ��ȡһ�����е��ļ��򿪱���
int find_father_dir(int fd); // Ѱ��һ�����ļ��ĸ�Ŀ¼���ļ�
void show_help(); // ��ȡ�������
void error(char *command); // ���������Ϣ


unsigned char *myvhard;

unsigned char *v_addr0; // ָ��������̵���ʼ��ַ

useropen openfilelist[MAXOPENFILE]; //�û����ļ�������

char currentdir[80];

int currfd; // ��¼��ǰĿ¼���ļ������� fd

int curdir;

unsigned char *startp;

unsigned char buffer[SIZE]; // ������