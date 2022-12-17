cmake_minimum_required(VERSION 3.20.2)
project(FileSystem C)

set(CMAKE_C_STANDARD 11)

add_executable(FileSystem
        fs_main.c
        fs.h
        Function2_startsys.c
        Function3_my_format.c
        Function4_my_cd.c
        Function5_my_rmdir.c
        Function6_my_mkdir.c
        Function7_my_ls.c
        Function8_my_create.c
        Function9_my_rm.c
        Function10_my_open.c
        Function11_my_close.c
        Function12_my_write.c
        Function13_do_write.c
        Function14_my_read.c
        Function15_do_read.c
        Function16_my_exitsys.c
        utils.c
        )
