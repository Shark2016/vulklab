#include <linux/miscdevice.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include "vulklab.h"

static int vulkdev_open(struct inode *inode, struct file *file)
{
	return 0;
}

static int vulkdev_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int vulkdev_mmap(struct file *fp, struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "VULKLAB: vulkdev_mmap addr=%lx, pfn=%lx, size=%lx\n",
			vma->vm_start, vma->vm_pgoff, vma->vm_end - vma->vm_start);
	if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff,
		vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

struct file_operations vulkdev3_ops = {
	.owner		= THIS_MODULE,
	.release    = vulkdev_release,
	.open       = vulkdev_open,
	.mmap       = vulkdev_mmap,
};
