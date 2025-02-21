#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/miscdevice.h>

MODULE_AUTHOR("mvm");
MODULE_DESCRIPTION("mvm");
MODULE_LICENSE("GPL");

#define MVMS 10



struct mvm {
    uint64_t Mvm;
    uint64_t mVm;
    uint64_t mvM;
    struct mvm* mvm;
} __attribute__ ((packed));

static DEFINE_SPINLOCK(mvm_lock);

struct mvm stored_mvms[MVMS] = {};

static void store_mvms(struct mvm* mvms)
{
    int i = 0;
    for (i = 0; i < MVMS && mvms; i++) {
        stored_mvms[i] = *mvms;
        stored_mvms[i].mvm = &stored_mvms[i + 1];
        mvms = mvms->mvm;
    }
}

static void load_mvms(struct mvm mvms[MVMS])
{
    int i = 0;
    struct mvm* curr_mvm = stored_mvms;
    for (i = 0; i < MVMS && curr_mvm; i++) {
        mvms[i] = *curr_mvm;
        mvms[i].mvm = &mvms[i + 1];
        curr_mvm = curr_mvm->mvm;
    }
}

static long mvm_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct mvm mvms[MVMS] = {};
    spin_lock(&mvm_lock);
    
    if (cmd == 0x6d766d) {

        struct __user mvm *curr_mvm = (struct __user mvm *) arg;
        int i = 0;


        for (i = 0; curr_mvm; i++) {
            if (copy_from_user(&mvms[i], curr_mvm, sizeof(struct mvm))) goto err;
            
            curr_mvm = mvms[i].mvm;
            if (i > 0) {
                mvms[i - 1].mvm = &mvms[i];
            }
        }
        
        store_mvms(mvms);

        spin_unlock(&mvm_lock);
        return i * sizeof(struct mvm);

    } else if (cmd == 0x4d564d) {

        load_mvms(mvms);

        struct __user mvm *out = (struct __user mvm*) arg;
        struct mvm* curr_mvm = mvms;
        int i = 0;

        for (i = 0; curr_mvm; i++) {
            if (copy_to_user(&out[i], curr_mvm, sizeof(struct mvm))) goto err;
            curr_mvm = curr_mvm->mvm;
        }

        spin_unlock(&mvm_lock);
        return i * sizeof(struct mvm);

    }

err:
    spin_unlock(&mvm_lock);
    return -EFAULT;
}

static struct file_operations mvm_fops = {
    .unlocked_ioctl = mvm_ioctl
};

struct miscdevice mvm_dev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mvm",
    .fops = &mvm_fops,
};

static int __init mvm_init(void)
{
    printk(KERN_INFO "mvm");
    if (misc_register(&mvm_dev) < 0) {
        printk(KERN_INFO "mvm :(\n");
		return -1;
	}
    return 0;
}

static void __exit mvm_exit(void)
{
    printk(KERN_INFO "mvm...");
}

module_init(mvm_init);
module_exit(mvm_exit);