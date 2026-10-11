#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/uaccess.h>
#include <linux/atomic.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/errno.h>

#define DEVICE_NAME        "counter"
#define PROC_NAME          "counter_info"
#define COUNTER_MAX_INPUT  32

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Hoang Trung Hai - SS204779");
MODULE_DESCRIPTION("Assignment 01 Task 2: Atomic Counter Misc Device Driver");

static atomic_t counter;
static atomic_t read_counter;
static atomic_t write_counter;

/* =========================================================
 * Read: return current counter, then increment it.
 * ========================================================= */
static ssize_t counter_read(struct file *file,
                            char __user *buf,
                            size_t len,
                            loff_t *ppos)
{
    char kbuf[32];
    int value;
    int count;

    if (*ppos != 0)
        return 0;

    value = atomic_read(&counter);
    count = scnprintf(kbuf, sizeof(kbuf), "%d\n", value);

    if (len < count)
        return -EINVAL;

    if (copy_to_user(buf, kbuf, count))
        return -EFAULT;

    atomic_inc(&counter);
    atomic_inc(&read_counter);

    *ppos += count;

    return count;
}

/* =========================================================
 * Write: accept a non-negative integer and set the counter.
 * ========================================================= */
static ssize_t counter_write(struct file *file,
                             const char __user *buf,
                             size_t len,
                             loff_t *ppos)
{
    char kbuf[COUNTER_MAX_INPUT];
    int value;
    int ret;

    if (len == 0 || len >= sizeof(kbuf))
        return -EINVAL;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    ret = kstrtoint(kbuf, 10, &value);
    if (ret < 0)
        return -EINVAL;

    if (value < 0)
        return -EINVAL;

    atomic_set(&counter, value);
    atomic_inc(&write_counter);

    return len;
}

/* =========================================================
 * File operations
 * ========================================================= */
static const struct file_operations counter_fops = {
    .owner  = THIS_MODULE,
    .read   = counter_read,
    .write  = counter_write,
    .llseek = no_llseek,
};

/* =========================================================
 * Misc device: /dev/counter
 * ========================================================= */
static struct miscdevice counter_miscdev = {
    .minor = MISC_DYNAMIC_MINOR,
    .name  = DEVICE_NAME,
    .fops  = &counter_fops,
    .mode  = 0666,
};

/* =========================================================
 * Procfs: /proc/counter_info
 * ========================================================= */
static int counter_proc_show(struct seq_file *m, void *v)
{
    seq_printf(m, "Current counter: %d\n",
               atomic_read(&counter));

    seq_printf(m, "Total reads/increments: %d\n",
               atomic_read(&read_counter));

    seq_printf(m, "Total writes/resets: %d\n",
               atomic_read(&write_counter));

    return 0;
}

static int counter_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, counter_proc_show, NULL);
}

static const struct proc_ops counter_proc_ops = {
    .proc_open    = counter_proc_open,
    .proc_read    = seq_read,
    .proc_lseek   = seq_lseek,
    .proc_release = single_release,
};

/* =========================================================
 * Module initialization
 * ========================================================= */
static int __init counter_init(void)
{
    int ret;

    atomic_set(&counter, 0);
    atomic_set(&read_counter, 0);
    atomic_set(&write_counter, 0);

    ret = misc_register(&counter_miscdev);
    if (ret) {
        pr_err("counter: failed to register misc device: %d\n", ret);
        return ret;
    }

    if (!proc_create(PROC_NAME, 0444, NULL, &counter_proc_ops)) {
        pr_err("counter: failed to create /proc/%s\n", PROC_NAME);
        misc_deregister(&counter_miscdev);
        return -ENOMEM;
    }

    pr_info("counter: driver initialized successfully\n");
    pr_info("counter: device /dev/%s registered\n", DEVICE_NAME);

    return 0;
}

/* =========================================================
 * Module cleanup
 * ========================================================= */
static void __exit counter_exit(void)
{
    remove_proc_entry(PROC_NAME, NULL);
    misc_deregister(&counter_miscdev);

    pr_info("counter: driver unloaded\n");
}

module_init(counter_init);
module_exit(counter_exit);
