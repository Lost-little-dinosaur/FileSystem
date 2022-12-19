#include "fs.h"

//Linux�ļ�����ϵͳ��myFormat��һ��������ʽ���洢�豸�ĺ�����
// ������������ʼ���洢�豸�ϵ��ļ�ϵͳ�������ļ�ϵͳ�ṹ��������Ŀ¼���ļ���Ŀ¼����Ϊ�ļ���Ŀ¼����ռ䡣
// ��ʽ��֮�󣬴洢�豸�Ϳ��������洢�ļ���Ŀ¼�ˡ�
// �ú������ܻ����һ����������������ָ����ʽ���ķ�ʽ��Ŀ��洢�豸��
void my_format() {
    memset(openfilelist, 0, sizeof(useropen) * MAXOPENFILE);
    startsys();
    // 1. ��������̵�һ������Ϊ�����飬��ʼ�� 8 ���ֽ����ļ�ϵͳ��ħ������Ϊ��10101010����
    // ��֮��д���ļ�ϵͳ��������Ϣ���� FAT ���С��λ�á���Ŀ¼��С��λ�á��̿��С���̿���������������ʼλ�õ���Ϣ��
    // 2. �����������������ȫһ���� FAT �����ڼ�¼�ļ���ռ�ݵĴ��̿鼰����������̿�ķ��䣬ÿ�� FAT ռ���������̿飻
    // ����ÿ�� FAT �У�ǰ�� 5 ��������Ϊ�ѷ��䣬���� 995 ��������Ϊ���У�
    // 3. �ڵڶ��� FAT �󴴽���Ŀ¼�ļ� root�����������ĵ� 1 �飨��������̵ĵ� 6 �飩�������Ŀ¼�ļ���
    // �ڸô����ϴ������������Ŀ¼���.���͡�..���������ݳ����ļ�����֮ͬ�⣬�����ֶ���ȫ��ͬ��
    // 4. ����������е����ݱ��浽 myfsys �ļ��У�ת5
    // 5. ʹ�� c ���ԵĿ⺯�� fclose()�ر� myfsys �ļ���
    //�ܽ�:
    //1. ����������(1 ���̿�)
    //2. ���� FAT1(2 ���̿�) FAT2(2 ���̿�)
    //3. ���ø�Ŀ¼�ļ�����������Ŀ¼��.��..(��Ŀ¼�ļ�ռһ���̿�,����Ŀ¼������д������̿������)��1���̿顢�� 6 �飩
    //��������Ϣ
    block0 *guideBlock = (block0 *) v_addr0;
    strcpy(guideBlock->magic_number, "10101010"); //ħ����magic number����ָһ����������֣���������ʶ�ļ������ͻ��ʽ�����ļ�ϵͳ�У�ħ��ͨ��������ʶ���ļ�ϵͳ�����͡�
    strcpy(guideBlock->information, "�ļ�ϵͳ,�����䷽ʽ:FAT,���̿ռ����:�����FAT ��λʾͼ,Ŀ¼�ṹ:���û��༶Ŀ¼�ṹ.");
    guideBlock->root = 5;
    guideBlock->startBlock = v_addr0 + BLOCKSIZE * 5;
    //�������� FAT ����Ϣ
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE); //FAT1�ӵڶ����̿鿪ʼ����һ���̿���������
    for (int i = 0; i < 5; i++) { //ǰ 5 ������Ϊ�ѷ��� END �� 995 ������Ϊ���� FREE
        fat1[i].id = END;
    }
    for (int i = 5; i < 1000; i++) {
        fat1[i].id = FREE;
    }
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3); //FAT2�ӵ��ĸ��̿鿪ʼ
    fat1[5].id = END; //5 ���̿鱻��Ŀ¼��ռ����
    memcpy(fat2, fat1, BLOCKSIZE); //�� FAT1 ���Ƶ� FAT2
    //��Ŀ¼���� fcb,����Ŀ¼��., ָ���Լ�
    fcb *root = (fcb *) (v_addr0 + BLOCKSIZE * 5);
    strcpy(root->filename, "."); //������ǰĿ¼
    strcpy(root->exname, "di"); //�ļ���չ��
    root->metadata = 0;// 0 ��ʾĿ¼�ļ�
    time_t rawTime = time(NULL); //��ȡ��ǰʱ��
    struct tm *tmTime = localtime(&rawTime); //localtime()���ڽ�time_tֵת��Ϊstruct tm���͵�����ʱ��ֵ
    //root->tmTime �� unsigned short int ���͵�����, 32 λ�������³� 16 λ,64 λ�������³� 32 λ
    //���￼�� 32 λ������, ���������ı�ʾһ��ʱ��������ǲ�������,����,����������һ��
    //����Сʱռ 5 λ,����ռ 6 λ,��ռ 5 λ
    root->time = tmTime->tm_hour * 2048 + tmTime->tm_min * 32 + tmTime->tm_sec / 2;
    root->date = (tmTime->tm_year - 100) * 512 + (tmTime->tm_mon + 1) * 32 +
                 (tmTime->tm_mday);
    root->first = 5; //��Ŀ¼���� fcb,����Ŀ¼��., ָ���Լ�
    root->free = 1; //��ʾ�ѷ���
    root->length = 2 * sizeof(fcb);
    //root2 ָ���Ŀ¼���ĵڶ��� fcb,������Ŀ¼��..
    fcb *root2 = root + 1; //��Ϊ��Ŀ¼��û���ϼ�Ŀ¼,����ָ���Լ�
    memcpy(root2, root, sizeof(fcb)); //����Ŀ¼���ĵ�һ�� fcb ���Ƶ��ڶ��� fcb
    strcpy(root2->filename, ".."); //�����ϼ�Ŀ¼��..
    for (int i = 0; i < (int) (BLOCKSIZE / sizeof(fcb)); i++) { //��ʼ����Ŀ¼���� fcb
        root2++;
        strcpy(root2->filename, "");
        root2->free = 0; //��ʾ����
    }
    //д���ļ�����ȥ
    FILE *fp = fopen(FILENAME, "w");
    fwrite(v_addr0, SIZE, 1, fp);
    fclose(fp);
}