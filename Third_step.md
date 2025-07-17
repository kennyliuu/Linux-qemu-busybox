## 🔧 Third step：Build and Test DebugFS Kernel Module

DebugFS provides a simple way for kernel code to export information to userspace, commonly used for debugging and development.

### ✅ [1/5] Create DebugFS Module Source Code

Create a kernel module that provides a virtual file interface through the debugfs filesystem, when system read the file, it returns "Hello from debugfs!"

```c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/string.h>

#define DIR_NAME "hello_dir"
#define FILE_NAME "hello_debugfs"

static struct dentry *dir;
static struct dentry *file;

static ssize_t hello_read(struct file *filp, char __user *buffer, size_t len, loff_t *offset)
{
    const char *msg = "Hello from debugfs!\n";
    return simple_read_from_buffer(buffer, len, offset, msg, strlen(msg));  // Helper function to safely copy data to userspace
}

static const struct file_operations fops = {    // Defines what happens when userspace reads/writes the file
    .owner = THIS_MODULE,
    .read = hello_read,
};

static int __init hello_init(void)
{
    dir = debugfs_create_dir(DIR_NAME, NULL);    // Creates a directory in /sys/kernel/debug/
    if (!dir) {
        pr_err("Failed to create debugfs directory\n");
        return -ENOMEM;
    }

    file = debugfs_create_file(FILE_NAME, 0666, dir, NULL, &fops);    // Creates a virtual file with custom read/write operations
    if (!file) {
        pr_err("Failed to create debugfs file\n");
        debugfs_remove(dir);
        return -ENOMEM;
    }

    pr_info("hello_debugfs loaded\n");
    return 0;
}

static void __exit hello_exit(void)
{
    debugfs_remove_recursive(dir);
    pr_info("hello_debugfs unloaded\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("A simple debugfs hello module");
```
Makefile:

```makefile
obj-m += hello_debugfs.o

all:
	make -C $(KERNEL_DIR) M=$(PWD) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- modules

clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
```


---
### ✅ [2/5] Compile Module

Generate the loadable kernel module (.ko file) that can interact with the debugfs filesystem.

```bash
make -C linux ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=hello_debugfs modules
```

---

### ✅ [3/5] Install Module in Root Filesystem

Make the debugfs module available in the virtual machine and configure it to load automatically at boot.

```bash
sudo mkdir -p mnt
sudo mount rootfs.img mnt
sudo mkdir -p mnt/lib/modules
sudo cp hello_debugfs/hello_debugfs.ko mnt/lib/modules/
sudo sed -i '/exec \/bin\/sh/i\\insmod /lib/modules/hello_debugfs.ko' mnt/etc/init.d/rcS
sudo umount mnt
```
---

### ✅ [4/5] Configure DebugFS Mount Point

Mount the debugfs virtual filesystem so our module's files become accessible in userspace.

```bash
sudo mount rootfs.img mnt
# Add debugfs mount to rcS if not already present
sudo sed -i '/mount -t sysfs/a\\mount -t debugfs none /sys/kernel/debug' mnt/etc/init.d/rcS
sudo umount mnt
```
---

### ✅ [5/5] Start QEMU and Test

Verify that our debugfs module works correctly and provides the expected interface.

Activate QEMU:
```bash
qemu-system-aarch64 \
  -M virt \
  -cpu cortex-a53 \
  -m 512M \
  -nographic \
  -bios /home/kenny/minimal-linux2/u-boot/u-boot.bin \
  -kernel /home/kenny/minimal-linux2/linux/arch/arm64/boot/Image \
  -append "root=/dev/vda rw console=ttyAMA0 init=/init" \
  -drive if=virtio,format=raw,file=/home/kenny/minimal-linux2/rootfs.img
```
Testing Commands:
```bash
# Check if debugfs is mounted
mount -t debugfs none /sys/kernel/debug

# Verify our module created the directory and file
ls -la /sys/kernel/debug/hello_dir/

# Test reading from our debugfs file
cat /sys/kernel/debug/hello_dir/hello_debugfs

# Check kernel messages
dmesg | grep hello_debugfs
```
output:
```bash
~ # ls /sys/kernel/debug/hello_dir/
hello_debugfs

~ # cat /sys/kernel/debug/hello_dir/hello_debugfs
Hello from debugfs!

~ # dmesg | grep hello_debugfs
[    0.982926] hello_debugfs: loading out-of-tree module taints kernel.
[    0.987144] hello_debugfs loaded
```
