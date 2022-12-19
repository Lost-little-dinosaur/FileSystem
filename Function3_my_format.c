#include "fs.h"

//Linux文件管理系统的myFormat是一个用来格式化存储设备的函数。
// 它可以用来初始化存储设备上的文件系统，创建文件系统结构，包括根目录、文件和目录，并为文件和目录分配空间。
// 格式化之后，存储设备就可以用来存储文件和目录了。
// 该函数可能会接受一个或多个参数，用于指定格式化的方式和目标存储设备。
void my_format() {
    memset(openfilelist, 0, sizeof(useropen) * MAXOPENFILE);
    startsys();
    // 1. 将虚拟磁盘第一个块作为引导块，开始的 8 个字节是文件系统的魔数，记为“10101010”；
    // 在之后写入文件系统的描述信息，如 FAT 表大小及位置、根目录大小及位置、盘块大小、盘块数量、数据区开始位置等信息；
    // 2. 在引导块后建立两张完全一样的 FAT 表，用于记录文件所占据的磁盘块及管理虚拟磁盘块的分配，每个 FAT 占据两个磁盘块；
    // 对于每个 FAT 中，前面 5 个块设置为已分配，后面 995 个块设置为空闲；
    // 3. 在第二张 FAT 后创建根目录文件 root，将数据区的第 1 块（即虚拟磁盘的第 6 块）分配给根目录文件，
    // 在该磁盘上创建两个特殊的目录项：“.”和“..”，其内容除了文件名不同之外，其他字段完全相同。
    // 4. 将虚拟磁盘中的内容保存到 myfsys 文件中；转5
    // 5. 使用 c 语言的库函数 fclose()关闭 myfsys 文件；
    //总结:
    //1. 设置引导块(1 个盘块)
    //2. 设置 FAT1(2 个盘块) FAT2(2 个盘块)
    //3. 设置根目录文件的两个特殊目录项.和..(根目录文件占一个盘块,两个目录项是在写在这个盘块里面的)（1个盘块、第 6 块）
    //引导块信息
    block0 *guideBlock = (block0 *) v_addr0;
    strcpy(guideBlock->magic_number, "10101010"); //魔数（magic number）是指一个特殊的数字，它用来标识文件的类型或格式。在文件系统中，魔数通常被用来识别文件系统的类型。
    strcpy(guideBlock->information, "文件系统,外存分配方式:FAT,磁盘空间管理:结合于FAT 的位示图,目录结构:单用户多级目录结构.");
    guideBlock->root = 5;
    guideBlock->startBlock = v_addr0 + BLOCKSIZE * 5;
    //设置两个 FAT 表信息
    fat *fat1 = (fat *) (v_addr0 + BLOCKSIZE); //FAT1从第二个盘块开始，第一个盘块是引导块
    for (int i = 0; i < 5; i++) { //前 5 个设置为已分配 END 后 995 个设置为空闲 FREE
        fat1[i].id = END;
    }
    for (int i = 5; i < 1000; i++) {
        fat1[i].id = FREE;
    }
    fat *fat2 = (fat *) (v_addr0 + BLOCKSIZE * 3); //FAT2从第四个盘块开始
    fat1[5].id = END; //5 号盘块被根目录区占用了
    memcpy(fat2, fat1, BLOCKSIZE); //将 FAT1 复制到 FAT2
    //根目录区的 fcb,特殊目录项., 指向自己
    fcb *root = (fcb *) (v_addr0 + BLOCKSIZE * 5);
    strcpy(root->filename, "."); //建立当前目录
    strcpy(root->exname, "di"); //文件扩展名
    root->metadata = 0;// 0 表示目录文件
    time_t rawTime = time(NULL); //获取当前时间
    struct tm *tmTime = localtime(&rawTime); //localtime()用于将time_t值转换为struct tm类型的日期时间值
    //root->tmTime 是 unsigned short int 类型的数据, 32 位编译器下长 16 位,64 位编译器下长 32 位
    //这里考虑 32 位编译器, 所以完整的表示一个时间的秒数是不够长的,所以,保存秒数的一半
    //这样小时占 5 位,分钟占 6 位,秒占 5 位
    root->time = tmTime->tm_hour * 2048 + tmTime->tm_min * 32 + tmTime->tm_sec / 2;
    root->date = (tmTime->tm_year - 100) * 512 + (tmTime->tm_mon + 1) * 32 +
                 (tmTime->tm_mday);
    root->first = 5; //根目录区的 fcb,特殊目录项., 指向自己
    root->free = 1; //表示已分配
    root->length = 2 * sizeof(fcb);
    //root2 指向根目录区的第二个 fcb,即特殊目录项..
    fcb *root2 = root + 1; //因为根目录区没有上级目录,所以指向自己
    memcpy(root2, root, sizeof(fcb)); //将根目录区的第一个 fcb 复制到第二个 fcb
    strcpy(root2->filename, ".."); //建立上级目录：..
    for (int i = 0; i < (int) (BLOCKSIZE / sizeof(fcb)); i++) { //初始化根目录区的 fcb
        root2++;
        strcpy(root2->filename, "");
        root2->free = 0; //表示空闲
    }
    //写到文件里面去
    FILE *fp = fopen(FILENAME, "w");
    fwrite(v_addr0, SIZE, 1, fp);
    fclose(fp);
}