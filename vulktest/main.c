#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "vulklab.h"

void vulk_list(void)
{
    int i = 0;

    while (vulk_entries[i].name) {
        printf("[+] %s\t%s\n", vulk_entries[i].name, vulk_entries[i].desc);
        i++;
    }
}

int vulk_run(char *cmd, int argc, char *argv[])
{
    int i = 0;

    while (vulk_entries[i].name) {
        if (!strcmp(cmd, vulk_entries[i].name))
            return vulk_entries[i].vulk_main(argc, argv);
        i++;
    }
    printf("no such vulkdev exploit\n");
    return 0;
}

void help()
{
    printf("\tvulkdev_exp -h\n");
    printf("\tvulkdev_exp -l\n");
    printf("\tvulkdev_exp -r vulkdev_exp1\n\n");
}

int main(int argc, char *argv[])
{
    int ch;
    char *cmd = NULL;

    while ((ch = getopt(argc, argv, "hlr:")) != -1) {
        switch (ch) {
        case 'l':
            vulk_list();
            return 0;
        case 'r':
            cmd = optarg;
            break;
        }
    }

    if (cmd && (argc > 2))
        return vulk_run(cmd, argc - 2, &argv[2]);

    help();
    return 0;
}