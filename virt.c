#include <linux/module.h>
// #include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
// #include <linux/errno.h>	/* error codes */
// #include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
// #include <linux/fcntl.h>	/* O_ACCMODE */
// #include <linux/seq_file.h>
#include <linux/cdev.h>
#include "virt.h"
#include <linux/uaccess.h>	/* copy_*_user */

MODULE_LICENSE("MIT");
MODULE_AUTHOR("Ananth Bashyam");

dev_t dev;
int VIRT_Q_SET = 1000;
int VIRT_QUANTUM = 40;
int majorNumber;
int num_devices = 1;
struct virt_dev *devices;
int MAJOR_NUM = 300;
int MINOR_NUM = 0;

static int virt_init(void){
    printk(KERN_ALERT "init!\n");
	int start = register_chrdev(MAJOR_NUM,"virt",&virt_fops);//register_chrdev_region(dev, num_devices, "virt");
	if(start < 0){
        printk(KERN_ALERT "FAIL");
    }else{
        printk(KERN_ALERT "Driver has started"); 
    }
    
    //initialize device and dev
    devices = kmalloc(num_devices * sizeof(struct virt_dev),GFP_KERNEL); //reserve memory for driver
    memset(devices,0,num_devices * sizeof(struct virt_dev));
    devices[0].quantum = VIRT_QUANTUM;
    devices[0].qset = VIRT_Q_SET;
    return 0;
}

static void virt_setup(struct virt_dev *dev,int device_num){
    //get the minor device number
    int err,devNo = MKDEV(MAJOR_NUM,device_num);
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
    // for(i;i<num_devices;i++){
    //     cdev_del(&devices[i].cdev);
    // }
    //free memory
    kfree(devices);
    unregister_chrdev(MAJOR_NUM,"virt");
    printk(KERN_ALERT "ENDED ALL DEVICES");
}

// free the data in the quantum set
// void clean_device(){
//     int i;
//     // for(i=0;i<num_devices;i++){
//     //     struct virt_qset vr =  devices[i].data
//     //     while (vr != NULL)
//     //     {
//     //         kfree(vr.data);
//     //         vr = vr.next;
//     //     }
        
//     // }
// }

static int virt_open(struct inode *inode, struct file *filp){
    //find the opened device through container of
    //struct virt_dev *dev_opened = container_of(inode->i_cdev,struct virt_dev ,cdev);
    filp->private_data = &devices[0];
    /*
    Trim file do later
    */
    return 0;
}
int virt_release(struct inode *inode, struct file *filp){
    return 0; // this is a virtual driver
}

struct virt_qset *get_data(struct virt_dev *dev,int n){
    return get_data_helper(dev->data,n,NULL);
    
}
struct virt_qset *get_data_helper(struct virt_qset *q,int n,struct virt_qset *prev){
    if(n < 0)return NULL;
    if(q == NULL){
        //create a new qset
        q = kmalloc(sizeof(struct virt_qset),GFP_KERNEL);
        memset(q,0,sizeof(struct virt_qset));
        printk("First Null Test %d %d",!q,!q->data);
        if(prev != NULL && prev->next == NULL){
            prev->next = q;
        }
        if(n == 0)return q;
        return get_data_helper(q->next,n-1,q);
    }else{
        if(prev != NULL && prev->next == NULL){
            prev->next = q;
        }
        if(n == 0)return q;
        return get_data_helper(q->next,n-1,q);
    }
    return NULL;
}

static ssize_t virt_read(struct file *filp, char *buf, size_t count,loff_t *f_pos){
    //struct virt_dev *device = filp->private_data;
    
    //if(device->size <= *f_pos)return -1;
    // if(device->size + count <= *f_pos){
    //     count = *f_pos - device->size + count;
    // }
    printk("IN READ");
    long quantum_area = (VIRT_Q_SET*VIRT_QUANTUM);
    int dev_pos = (int)(*f_pos/quantum_area); // gets which device its in
    int position_in_q_set = (int)(*f_pos % quantum_area) / VIRT_QUANTUM;
    int position_in_quantum =  (int)(*f_pos % quantum_area) % VIRT_QUANTUM;
    struct virt_dev *w = filp->private_data;
    printk("Second Null Tests %d",w->data == NULL);
    struct virt_qset *dataPtr = get_data(filp->private_data,dev_pos);
    if(dataPtr == NULL || dataPtr->data == NULL || dataPtr->data[position_in_q_set] == NULL){
        printk(KERN_ALERT "FAIL");
        printk(dataPtr == NULL ? "Data pointer is null" : "data pointer is not null");
        printk(dataPtr->data == NULL ? "Data pointer to data is null" : "Data pointer to data is not null");
        
        return 0;
    }
    if(count > (VIRT_QUANTUM - position_in_quantum)){
        count = position_in_quantum;
    }
    printk("IN READ FUNCTION %d %c",count,*(char *)(dataPtr->data[position_in_q_set]));
    
    int res = copy_to_user(buf,dataPtr->data[position_in_q_set],count);
    *f_pos+=count;
    return count;
}
static ssize_t virt_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos){
    struct virt_dev *device = filp->private_data;
    long quantum_area = (VIRT_Q_SET*VIRT_QUANTUM);
    struct virt_dev *w = filp->private_data;
    int dev_pos = (int)(*f_pos/quantum_area); // gets which device its in
    int position_in_q_set = (int)(*f_pos % quantum_area) / VIRT_QUANTUM;
    int position_in_quantum =  (int)(*f_pos % quantum_area) % VIRT_QUANTUM;
    struct virt_qset *dataPtr = get_data(device,dev_pos);
    if(dataPtr == NULL){
        printk(KERN_ALERT "Failed to initilize data aborting");
        return -1;
    }
    /*If the quantum set was not initialized before */
    if(!dataPtr->data){
        dataPtr->data = kmalloc(VIRT_Q_SET*sizeof(char *),GFP_KERNEL);
        if(!dataPtr->data){
            printk(KERN_ALERT "memset fail");
        }
        memset(dataPtr->data,0,VIRT_Q_SET*sizeof(char *));
    }
    /*If the quantum set was not initilized before*/
    if(!dataPtr->data[position_in_q_set]){
        dataPtr->data[position_in_q_set] = kmalloc(VIRT_QUANTUM*sizeof(char *),GFP_KERNEL);
    }
    if(count > VIRT_QUANTUM -  position_in_quantum){
        count =(VIRT_QUANTUM -  position_in_quantum);
    }
    copy_from_user(dataPtr->data[position_in_q_set]+position_in_quantum,buf,count);
    *f_pos+=count;
    w->data = dataPtr;
    printk(KERN_ALERT "Wrote %d bytes",count);
    printk("First Letter = %c",*(char *)dataPtr->data[position_in_q_set]+position_in_quantum);
    return count;
}

module_init(virt_init);
module_exit(virt_exit);
