#include <linux/miscdevice.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include "vulklab.h"

#define MAX_BUF 32

static int vulkdev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t vulkdev_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t vulkdev_write(struct file * file, const char __user * buf, 
			  size_t count, loff_t *ppos)
{
	char sbuf[MAX_BUF];
	if (copy_from_user(sbuf, buf, count))
		return -EFAULT;
	return count;
}

static int vulkdev_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations vulkdev2_ops = {
	.owner		= THIS_MODULE,
	.release    = vulkdev_release,
	.open       = vulkdev_open,
	.read       = vulkdev_read,
	.write      = vulkdev_write,
};
