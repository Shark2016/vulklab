
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vulklab.h"

extern int kernel_mmap1(int argc, char *argv[]);
extern int kernel_mmap2(int argc, char *argv[]);

struct vulk_entry vulk_entries[] = {
    {
        .name = "kernel_mmap1",
        .desc = "kernel mmap + setresuid patch",
        .vulk_main = kernel_mmap1,
    },
    {   .name = "kernel_mmap2",
        .desc = "kernel mmap + cred modify",
        .vulk_main = kernel_mmap2,
    },
    {
        .name = NULL
    }
};

