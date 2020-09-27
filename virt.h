
#include <linux/cdev.h>

struct virt_dev {
	struct virt_qset *data;   // Pointer to data
	int quantum;		   // Quantum size
	int qset;                  // No of quantums in each struct
	unsigned long size;        // Total size of device
	unsigned int access_key;   // used by sculluid and scullpriv(Beginners 
	struct cdev cdev;          // char device structure
};

struct virt_qset {
	void **data;
	struct virt_qset *next;
};
ssize_t virt_read(struct file *filp, char __user *buf, size_t count,loff_t *f_pos);
ssize_t virt_write(struct file *filp, const char __user *buf, size_t count,loff_t *f_pos);
static void virt_setup(struct virt_dev *dev,int device_num);
static int virt_open(struct inode *inode, struct file *filp);
virt_qset *get_data(struct virt_dev *dev,int n);
virt_qset *get_data_helper(struct virt_qset *q,int n);

struct file_operations virt_fops = {    
    .owner =    THIS_MODULE,
    // .llseek =   scull_llseek,
    .read =     virt_read,
    .write =    virt_write,
    // .ioctl =    scull_ioctl,
    .open =     virt_open,   
    .release =  virt_release,
};