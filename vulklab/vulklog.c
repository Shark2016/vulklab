#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include "vulklab.h"

static int vulklog_show(struct seq_file *m, void *v)
{
	seq_puts(m, "vulklog test\n");
	return 0;
}

static ssize_t vulklog_write(struct file *file, const char __user *ubuf,
	size_t count, loff_t *offp)
{
	return 0;
}

static int vulklog_open(struct inode *inode, struct file *file)
{
	return single_open(file, vulklog_show, NULL);
}

struct file_operations vulklog_fops = {
	.open = vulklog_open,
	.read = seq_read,
	.write = vulklog_write,
	.llseek = seq_lseek,
};

int vulklog_init(void)
{
	struct proc_dir_entry *pe;

	pe = proc_create("vulklog", 0440, NULL, &vulklog_fops);
	if (!pe)
		return -ENOMEM;
	return 0;
}


void vulklog_exit(void)
{
	remove_proc_entry("vulklog", NULL);
}