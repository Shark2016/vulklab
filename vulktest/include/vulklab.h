#ifndef __VULKLAB_H__
#define __VULKLAB_H__

struct vulk_entry {
    char *name;
    char *desc;
    int (*vulk_main)(int argc, char *argv[]);
};

extern struct vulk_entry vulk_entries[];
//#define VULK_ENTRY_COUNT  (sizeof(vulk_entries)/sizeof(vulk_entries[0]))

#endif /*__VULKLAB_H__*/
