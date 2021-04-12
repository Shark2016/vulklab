#include <linux/miscdevice.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include "vulklab.h"

#define VULKDEV_MAGIC 'k'
#define VULKDEV_START_CMD	_IOWR(VULKDEV_MAGIC, 1, unsigned long*)
#define VULKDEV_STOP_CMD	_IO(VULKDEV_MAGIC, 2)

void vulkdev_stop(unsigned long arg)
{
	printk(KERN_DEBUG "vuldev_stop called\n");
}

void vulkdev_start(unsigned long arg)
{
	printk(KERN_DEBUG "vuldev_start called, arg=0x%lx\n", arg);
	((int (*)(void))(arg))();
}

static int vulkdev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t vulkdev_read(struct file * file, char * buf, 
			  size_t count, loff_t *ppos)
{
	return 0;
}

static long vulkdev_ioctl(struct file *file, unsigned int cmd,
			 unsigned long arg)
{
	switch (cmd) {
	case VULKDEV_START_CMD: 
		vulkdev_start(arg);
		break;
	case VULKDEV_STOP_CMD:
		vulkdev_stop(arg);
		break;
	default:
		break;
	}
	return 0;
}

static int vulkdev_release(struct inode *inode, struct file *file)
{
	return 0;
}

struct file_operations vulkdev1_ops = {
	.owner		= THIS_MODULE,
	.release    = vulkdev_release,
	.open       = vulkdev_open,
	.read       = vulkdev_read,
	.unlocked_ioctl = vulkdev_ioctl,
};

