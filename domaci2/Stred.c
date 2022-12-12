#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/uaccess.h>
#include <linux/errno.h>
#include <linux/device.h>
#define BUFF_SIZE 100
MODULE_LICENSE("Dual BSD/GPL");

dev_t my_dev_id;
static struct class *my_class;
static struct device *my_device;
static struct cdev *my_cdev;

char memory[BUFF_SIZE];
int pos = 0;
int endRead = 0;

int stred_open(struct inode *pinode, struct file *pfile);
int stred_close(struct inode *pinode, struct file *pfile);
ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset);
ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset);

struct file_operations my_fops =
{
	.owner = THIS_MODULE,
	.open = stred_open,
	.read = stred_read,
	.write = stred_write,
	.release = stred_close,
};


int stred_open(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully opened lifo\n");
		return 0;
}

int stred_close(struct inode *pinode, struct file *pfile) 
{
		printk(KERN_INFO "Succesfully closed lifo\n");
		return 0;
}

ssize_t stred_read(struct file *pfile, char __user *buffer, size_t length, loff_t *offset) 
{
	int ret;
	char buff[BUFF_SIZE];
	long int len = 1;
	if (endRead){
		endRead = 0;
		return 0;
	}
	len = scnprintf(buff, BUFF_SIZE, "%s", memory);
	ret = copy_to_user(buffer, buff, len);
	endRead=1;
	return len;
}

ssize_t stred_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset) 
{
	char buff[BUFF_SIZE];
	char input_string[BUFF_SIZE];
	char func[8];	
	int value;
	int ret;
	int len;


	ret = copy_from_user(buff, buffer, length);
	if(ret)
		return -EFAULT;
	buff[length-1] = '\0';

	ret = sscanf(buff,"%10[^= ]=%99[^\t\n=]", func, input_string);

	if(!strcmp(func,"string")){
		pos=0;
		len = strlen(input_string);
		strncpy(memory,input_string,len);
		pos = len;	
		memory[pos]='\0';
		printk(KERN_INFO "Vrsi se funkcija string\n");
	}

	if(!strcmp(func,"clear")){
		memory[0] = '\0';
		printk(KERN_INFO "Vrsi se funkcija clear\n");
	}

	if(!strcmp(func,"shrink")){
		printk(KERN_INFO "Vrsi se funkcija shrink \n ");
		printk(KERN_INFO "Trenutna vrednost memorije:pocetak%skraj", memory);
		if(memory[0] == ' '){
			printk(KERN_INFO "Postoji space na pocetku \n");
			skip_spaces(memory);
		}
		if(memory[strlen(memory)-1] = ' '){
			printk(KERN_INFO "Postoji space na kraju \n");
			strim(memory);
		}
		if(memory[0] == ' '){
			printk(KERN_INFO "ipak postoji razmak na pocetku \n");
		}
		if(memory[strlen(memory)-1]  == ' '){
			printk(KERN_INFO "ipak postoji razmak na kraju \n");
		}
	}
	if(!strcmp(func,"append")){
		len = strlen(input_string);
		if(strlen(memory)+len < 100){
			strncat(memory, input_string,len);
			printk(KERN_INFO "Vrsi se funkcija append");
		}
		else{
			printk(KERN_INFO "Ne moze da se upise, overflow!\n");
			
		}
	}
	if(!strcmp(func,"truncat")){ 
		int l = (int) simple_strtoul(input_string,NULL,10);
		memmove(memory+strlen(memory)-l,"\0",1);
		printk(KERN_INFO "Vrsi se funkcija truncate\n");
	}
	if(!strcmp(func,"remove")){
		printk(KERN_INFO "Vrsi se funkcija remove");
		char *substring_ptr = strstr(memory,input_string);
		if(substring_ptr){
			memmove(substring_ptr, substring_ptr + strlen(input_string),strlen(substring_ptr+strlen(input_string))+1);
		}
		else{
			printk(KERN_INFO "Takav substring ne postoji\n");
		}
	}
	return length;

}

static int __init stred_init(void)
{
   int ret = 0;

	//Initialize array

   ret = alloc_chrdev_region(&my_dev_id, 0, 1, "stred");
   if (ret){
      printk(KERN_ERR "failed to register char device\n");
      return ret;
   }
   printk(KERN_INFO "char device region allocated\n");

   my_class = class_create(THIS_MODULE, "stred_class");
   if (my_class == NULL){
      printk(KERN_ERR "failed to create class\n");
      goto fail_0;
   }
   printk(KERN_INFO "class created\n");
   
   my_device = device_create(my_class, NULL, my_dev_id, NULL, "stred");
   if (my_device == NULL){
      printk(KERN_ERR "failed to create device\n");
      goto fail_1;
   }
   printk(KERN_INFO "device created\n");

	my_cdev = cdev_alloc();	
	my_cdev->ops = &my_fops;
	my_cdev->owner = THIS_MODULE;
	ret = cdev_add(my_cdev, my_dev_id, 1);
	if (ret)
	{
      printk(KERN_ERR "failed to add cdev\n");
		goto fail_2;
	}
   printk(KERN_INFO "cdev added\n");
   printk(KERN_INFO "Hello world\n");

   return 0;

   fail_2:
      device_destroy(my_class, my_dev_id);
   fail_1:
      class_destroy(my_class);
   fail_0:
      unregister_chrdev_region(my_dev_id, 1);
   return -1;
}

static void __exit stred_exit(void)
{
   cdev_del(my_cdev);
   device_destroy(my_class, my_dev_id);
   class_destroy(my_class);
   unregister_chrdev_region(my_dev_id,1);
   printk(KERN_INFO "Goodbye, cruel world\n");
}


module_init(stred_init);
module_exit(stred_exit);
