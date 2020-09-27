#include <linux/module.h>
// #include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
// #include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
// #include <linux/errno.h>	/* error codes */
// #include <linux/types.h>	/* size_t */
// #include <linux/proc_fs.h>
// #include <linux/fcntl.h>	/* O_ACCMODE */
// #include <linux/seq_file.h>
// #include <linux/cdev.h>

// #include <linux/uaccess.h>	/* copy_*_user */

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Ananth Bashyam");

dev_t dev;

static int virt_init(void){
    printk(KERN_ALERT "init!\n");
    int start = alloc_chrdev_region(&dev,0,2,"virt");
    int major;
    major = MAJOR(dev);
    if(start < 0){
        printk(KERN_ALERT "FAIL");
    }else{
        printk(KERN_ALERT "woof has started");
    }
    //initialize device and dev
    return 0;
}


static void virt_exit(void){
    unregister_chrdev_region(dev,2);
    printk(KERN_ALERT "woof has ended\n");
}

module_init(virt_init);
module_exit(virt_exit);
