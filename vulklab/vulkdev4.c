#include <linux/miscdevice.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <asm/xen/page.h>
#include "vulklab.h"

static int size = 0x10000;

static int vulkdev_open(struct inode *inode, struct file *filp)
{
	filp->private_data = kzalloc(size, GFP_KERNEL);
	if (filp->private_data == NULL)
		return -1;
	return 0;
}

static int vulkdev_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int vulkdev_mmap(struct file *filp, struct vm_area_struct *vma)
{
	if (remap_pfn_range(vma, vma->vm_start, virt_to_pfn(filp->private_data),
		vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

struct file_operations vulkdev4_ops = {
	.owner		= THIS_MODULE,
	.release    = vulkdev_release,
	.open       = vulkdev_open,
	.mmap       = vulkdev_mmap,
};
