#include <linux/miscdevice.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include "vulklab.h"

static int size = 0x1000;

void simple_vma_open(struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "VULKLAB: simple_vma_open, virt %lx, phys %lx\n",
		vma->vm_start, vma->vm_pgoff << PAGE_SHIFT);
}

void simple_vma_close(struct vm_area_struct *vma)
{
	printk(KERN_NOTICE "VULKLAB: simple_vma_close\n");
}

int simple_vma_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
{
	struct page *page = NULL;
	unsigned long offset;

	printk(KERN_NOTICE "VULKLAB: simple_vma_fault\n");
	printk(KERN_NOTICE "VULKLAB: vmf->pgoff: %lx, vma->vm_pgoff: %lx, sum: %lx, PAGE_SHIFT: %x\n",
		(unsigned long)vmf->pgoff,
		(unsigned long)vma->vm_pgoff,
		((vmf->pgoff << PAGE_SHIFT) + (vma->vm_pgoff << PAGE_SHIFT)),
		PAGE_SHIFT);

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	offset = (((unsigned long)vmf->virtual_address - vma->vm_start) + (
		vma->vm_pgoff << PAGE_SHIFT));
#else
	offset = (((unsigned long)vmf->address - vma->vm_start) + (
		vma->vm_pgoff << PAGE_SHIFT));
#endif
	if (offset > PAGE_SHIFT << 4)
		goto nopage_out;

	page = virt_to_page(vma->vm_private_data + offset);
	vmf->page = page;
	get_page(page);

nopage_out:
	return 0;
}

static struct vm_operations_struct simple_remap_vm_ops = {
	.open = simple_vma_open,
	.close = simple_vma_close,
	.fault = simple_vma_fault,
};

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
	vma->vm_private_data = filp->private_data;
	vma->vm_ops = &simple_remap_vm_ops;
	simple_vma_open(vma);

	printk(KERN_INFO "VULKLAB: device mmap ok\n");
	return 0;
}

struct file_operations vulkdev5_ops = {
	.owner		= THIS_MODULE,
	.release    = vulkdev_release,
	.open       = vulkdev_open,
	.mmap       = vulkdev_mmap,
};
