#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include "vulklab.h"

extern struct file_operations vulkdev1_ops;
extern struct file_operations vulkdev2_ops;
extern struct file_operations vulkdev3_ops;
extern struct file_operations vulkdev4_ops;
extern struct file_operations vulkdev5_ops;
extern struct file_operations vulkdev6_ops;

static struct miscdevice misc_dev1 = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "vulkdev1",
	.fops	= &vulkdev1_ops,
	.mode	= 0666,
};

static struct miscdevice misc_dev2 = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "vulkdev2",
	.fops	= &vulkdev2_ops,
	.mode	= 0666,
};

static struct miscdevice misc_dev3 = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "vulkdev3",
	.fops	= &vulkdev3_ops,
	.mode	= 0666,
};

static struct miscdevice misc_dev4 = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "vulkdev4",
	.fops	= &vulkdev4_ops,
	.mode	= 0666,
};

static struct miscdevice misc_dev5 = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= "vulkdev5",
	.fops	= &vulkdev5_ops,
	.mode	= 0666,
};

static int __init vulklab_init(void)
{
	if (vulklog_init())
		return -1;

	if (misc_register(&misc_dev1))
		goto err_dev1;
	if (misc_register(&misc_dev2))
		goto err_dev2;
	if (misc_register(&misc_dev3))
		goto err_dev3;
	if (misc_register(&misc_dev4))
		goto err_dev4;
	if (misc_register(&misc_dev5))
		goto err_dev5;
	return 0;

err_dev5:
	misc_deregister(&misc_dev4);
err_dev4:
	misc_deregister(&misc_dev3);
err_dev3:
	misc_deregister(&misc_dev2);
err_dev2:
	misc_deregister(&misc_dev1);
err_dev1:
	vulklog_exit();
	return -1;
}

static void __exit vulklab_exit(void)
{
	misc_deregister(&misc_dev5);
	misc_deregister(&misc_dev4);
	misc_deregister(&misc_dev3);
	misc_deregister(&misc_dev2);
	misc_deregister(&misc_dev1);
	vulklog_exit();
}

module_init(vulklab_init);
module_exit(vulklab_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sh4rk <chostrider90@gmail.com>");
MODULE_VERSION("0.1.0");
MODULE_DESCRIPTION("vulklab");

