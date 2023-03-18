#include <linux/module.h>
#include <linux/init.h>
#include <linux/moduleparam.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/pwm.h>
#include <linux/kernel.h>
#include <linux/delay.h>

#define LED_PIN 2
#define BTN_PIN 4
#define SIZE 15
#define SIZE_READ 15

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Saad ");
MODULE_DESCRIPTION("Servo motor driver ");
int number=0;
dev_t device_number;
module_param(number, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(number, "major number");
int major_number=0;
struct class *myclass;
struct device *mydevice;
struct cdev st_characterDevice;
static unsigned char buffer[SIZE]="";
struct pwm_device *pwm0 = NULL;
int button_value;

ssize_t driver_read(struct file *file, char __user *user_buffer, size_t count , loff_t *off){
     int not_copied;
     char tmp[SIZE_READ]="";
     printk("%s Driver read is called\n",__func__);
     printk("%s the offs is %lld\n",__func__,*off);
     printk("%s the count to read %ld\n",__func__,count);
     if(count + *off> SIZE_READ){
        count = SIZE_READ - *off;
     }

     not_copied = copy_to_user(user_buffer,&tmp[*off],count);
     if(not_copied){return -1;}
     *off = count;
     printk("%s not coped %d\n",__func__,not_copied);
     printk("%s message %s\n",__func__,user_buffer);
     return count;
}

ssize_t  driver_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *off){
     int not_copied;
     long value=0;
     printk("%s Driver write is called\n",__func__);
     printk("%s the offs is %lld\n",__func__,*off);
     printk("%s the count to read %ld\n",__func__,count);
     if(count +*off>SIZE){
        count = SIZE - *off;
     }
     if(!count){
      printk("no space left\n");
      return -1;
     }
     not_copied = copy_from_user(&buffer[*off],user_buffer,count);
     if(not_copied){return -1;}
       if (kstrtol_from_user(user_buffer, count - 1, 10, &value) == 0) // takes with ms
    {
        printk("driver_write - the value is %ld\n", value);
    }
    else
    {
        printk("failed- the value is %ld  length %ld  buff %s\n", value, strlen(user_buffer), user_buffer);
    }
    /* Set PWM on time */
    if (value > 1000) //
        printk("Invalid Value\n");
        pwm_config(pwm0, 1000000 + 1000000*(int)value /180, 20000000);  
    if (not_copied)
    {
        return -1;
    }
     *off = count;
     printk("%s message %s\n",__func__,buffer);

     return count;
}

static int driver_open(struct inode *i, struct file *f)   
{
  printk( "Driver: open()\n");
  return 0;
}

static int driver_close(struct inode *i, struct file *f)   
{
  printk("Driver: close()\n");
  return 0;
}

struct file_operations fops={
       .owner=THIS_MODULE,
       .open=driver_open,
       .release=driver_close,
       .read=driver_read,
       .write=driver_write
};

static int __init servo_driver_init(void)
{
    //1- device number
    int retvalue;
    retvalue=alloc_chrdev_region(&device_number,0,1,"servo_devicenumber");
    printk("hello kernel\n");
    if(retvalue==0){
          printk(" sucess registred major %d minor %d \n",MAJOR(device_number),MINOR(device_number));}
    else{printk("failed\n");return -1;}
    // 2- define driver type ch or bl ...
    cdev_init(&st_characterDevice,&fops);
    if( cdev_add(&st_characterDevice,device_number,1)!=0){
        unregister_chrdev_region(device_number,1);
        printk("Registering of devic to kernel failed\n");
        return -1;
        }
    //3- generate file (class)
    myclass=class_create(THIS_MODULE,"class_created");
     if((myclass==NULL)){
          printk("Device class can not be created\n");
          cdev_del(&st_characterDevice);
          unregister_chrdev_region(device_number,1);
          return -1;
          }
    mydevice= device_create(myclass,NULL,device_number,NULL,"servo_motor");
    if(mydevice==NULL){
        printk("Device class can not be created\n"); 
        cdev_del(&st_characterDevice);
        class_destroy(myclass);
        unregister_chrdev_region(device_number,1);
        return -1;
        }
 printk("device driver is created");
    pwm0 = pwm_request(0, "thepwm");
    if (pwm0 == NULL)
    {
        printk("Could not get PWM0!\n");
        device_destroy(myclass, device_number);
        return -1;
    }  
    pwm_enable(pwm0);  
     pwm_config(pwm0, 1000000, 20000000);// 1 ms pulse width, 20 MHz frequency angle =0 initial position
     msleep(200); // wait 200ms for the servo to rotate to 180 degrees  
 return 0; 
}

static void __exit driver_exit(void)
{   pwm_disable(pwm0);
    pwm_free(pwm0);
    cdev_del(&st_characterDevice);
    device_destroy(myclass,device_number);
    class_destroy(myclass);
    unregister_chrdev_region(device_number,1);
    printk("servo motor driver left!\n");
}
module_init(servo_driver_init);
module_exit(driver_exit);


