#include "fs.h"

void my_exitsys() {
    //当 currfd=0 的时候,也就是根节点, 它是不用更新的
    //因为我们之前在任何目录下 mkdir 或者 create 时,已经把 length 的变化写到目录文件下,名字叫.的目录项里了
    //即任何目录文件的'.'目录项都是实时更新的,但是他的父目录文件的没有被更新,所以需要一个个 close
    //但是根目录没有父目录文件, 所以不需要 close.
    while (currfd) {
        my_close(currfd);
    }
    FILE *fp = fopen(FILENAME, "w");
    fwrite(v_addr0, SIZE, 1, fp);
    fclose(fp);
}