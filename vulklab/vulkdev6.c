#include <linux/miscdevice.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include "vulklab.h"

int secret2 = 0xdeadbeef;

static int vulkdev_open(struct inode *inode, struct file *file)
{
	file->private_data = (void *)1;
	return 0;
}

static ssize_t vulkdev_read(struct file * file, char *buf,
			  size_t count, loff_t *ppos)
{
	char outbuf[256];
	ssize_t len, size = sizeof(outbuf);

	if (!file->private_data)
		return -EBADF;
	if (file->private_data != (void *)1)
		return 0;
	file->private_data = (void *)2;

	len = snprintf(outbuf, size, "secret=(%p)%lx\n", &secret2,
			(unsigned long)secret2);
	len += snprintf(outbuf + len, size - len, "addr_limit=%lx\n",
			current_thread_info()->addr_limit);
	len += snprintf(outbuf + len, size - len, "set_fs KERNEL_DS=%lx\n",
			(unsigned long)KERNEL_DS);

	set_fs(KERNEL_DS);
	// modify addr_limit directly is also available
	// 0 or large value like 0xffffffff are both ok
	// current_thread_info()->addr_limit = 0;
	// current_thread_info()->addr_limit = 0xffffffff;
	len += snprintf(outbuf + len, size - len, "addr_limit=%lx\n",
			current_thread_info()->addr_limit);
	if (len > count)
		return -EINVAL;
	if (copy_to_user(buf, outbuf, len))
		return -EFAULT;
	return len;
}

static ssize_t vulkdev_write(struct file * file, const char __user *buf,
			  size_t count, loff_t *ppos)
{
    return 0;
}

static int vulkdev_release(struct inode *inode, struct file *file)
{
	// restore addr_limit
	set_fs(USER_DS);
	return 0;
}

struct file_operations vulkdev6_ops = {
	.owner		= THIS_MODULE,
	.release    = vulkdev_release,
	.open       = vulkdev_open,
	.read       = vulkdev_read,
	.write      = vulkdev_write,
};
