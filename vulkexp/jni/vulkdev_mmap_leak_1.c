#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define VULKDEV_DEVICE "/dev/vulkdev5"

int main(int argc, char* argv[])
{
	int fd;
	uid_t uid;
	size_t size;
	unsigned int *addr;
	unsigned long start;
	off_t offset;

	fd = open(VULKDEV_DEVICE, O_RDWR);
	if (fd < 0) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	size = 0x100000;
	start = 0x42424000;
	addr = (unsigned int *)mmap((void *)start, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0x0);
	if (addr == MAP_FAILED) {
		printf("mmap failed, errno = %d\n", errno);
		close(fd);
		return -1;
	}
	printf("mmap addr: %lx\n", (unsigned long)addr);
	int i = 0;
	while ((unsigned long)addr < start + size - 0x10) {
		printf("%04d 0x%08lx 0x%08lx 0x%08lx 0x%08lx\n", i,
			(unsigned long)addr[0], (unsigned long)addr[1],
			(unsigned long)addr[2], (unsigned long)addr[3]);
		addr+=4;
		i++;
	}
	return 0;
}

