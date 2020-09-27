#include <linux/module.h>
// #include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
// #include <linux/errno.h>	/* error codes */
// #include <linux/types.h>	/* size_t */
// #include <linux/proc_fs.h>
// #include <linux/fcntl.h>	/* O_ACCMODE */
// #include <linux/seq_file.h>
#include <linux/cdev.h>
#include "virt.h"
#include <linux/uaccess.h>	/* copy_*_user */

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Ananth Bashyam");

dev_t dev;
int VIRT_Q_SET = 1000;
int VIRT_QUANTUM = 4000;
int majorNumber;
int num_devices = 2;
struct virt_dev *devices;
/*
Todo - 
implement data structures
implement open method
*/

static int virt_init(void){
    printk(KERN_ALERT "init!\n");
    int start = alloc_chrdev_region(&dev,0,num_devices,"virt"); //allocate the driver
    majorNumber = MAJOR(dev); // get a major number
    if(start < 0){
        printk(KERN_ALERT "FAIL");
    }else{
        printk(KERN_ALERT "woof has started"); 
    }
    //initialize device and dev
    devices = kmalloc(num_devices * sizeof(struct virt_dev),GFP_KERNEL); //reserve memory for driver
    memset(devices,0,num_devices * sizeof(struct virt_dev));
    int i;
    //initializes device to what we want
    for(i=0;i<num_devices;i++){
        devices[i].quantum = VIRT_QUANTUM;
        devices[i].qset = VIRT_Q_SET;
        virt_setup(&devices[i],i);
    }
    printk(KERN_ALERT "SETUP ALL DEVICES");
    return 0;
}

static void virt_setup(struct virt_dev *dev,int device_num){
    //get the minor device number
    int err,devNo = MKDEV(majorNumber,device_num);
    //Set the character driver up
    cdev_init(&dev->cdev,&virt_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &virt_fops;
    //add the device to the kernel
    err = cdev_add(&dev->cdev, devNo, 1);
    if(err){
        printk(KERN_ALERT "failed to add the character driver to the kernel abort abort");
    }

}

static void virt_exit(void){
    //get rid of character devices
    int i;
    for(i;i<num_devices;i++){
        cdev_del(&devices[i].cdev);
    }
    //free memory
    kfree(devices);
    unregister_chrdev_region(dev,2);
    printk(KERN_ALERT "ENDED ALL DEVICES");
}

// free the data in the quantum set
void clean_device(){
    int i;
    // for(i=0;i<num_devices;i++){
    //     struct virt_qset vr =  devices[i].data
    //     while (vr != NULL)
    //     {
    //         kfree(vr.data);
    //         vr = vr.next;
    //     }
        
    // }
}

static int virt_open(struct inode *inode, struct file *filp){
    //find the opened device through container of
    struct dev_opened = container_of(inode->i_cdev,struct virt_dev,cdev);
    filp->private_data = dev_opened;
    /*
    Trim file do later
    */
    return 0;
}
int virt_release(struct inode *inode, struct file *filp){
    return 0; // this is a virtual driver
}

virt_qset *get_data(struct virt_dev *dev,int n){
    return get_data_helper(dev->data,n);
}
virt_qset *get_data_helper(struct virt_qset *q,int n){
    if(n < 0)return NULL;
    if(q == NULL){
        //create a new qset
        q->data = kmalloc(sizeof(struct virt_qset),GFP_KERNEL);
        memset(q,0,sizeof(struct virt_qset));
        if(n == 0)return q;
        q->next = get_data_helper(q->next,n-1);
    }else{
        if(n == 0)return q;
        return get_data_helper(q->next,n-1);
    }
}

ssize_t virt_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos){
    
    //copy_to_user(buf,const void *from,count);
    return 0;
}
ssize_t virt_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos){
    return 0;
}

module_init(virt_init);
module_exit(virt_exit);
