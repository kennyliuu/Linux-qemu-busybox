## 👋 Second Step：Build and Test Hello Kernel Module

### ✅ [1/5] Create Module Source Code  `hello.c`

Write a basic kernel module that prints messages during load/unload operations.

```c
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_init(void) {
    pr_info("👋 Hello from kernel module!\n");
    return 0;
}

static void __exit hello_exit(void) {
    pr_info("👋 Goodbye from kernel module!\n");
}

module_init(hello_init);
module_exit(hello_exit);
MODULE_LICENSE("GPL");
```

Makefile：
Simple build configuration to compile the module as an object file.

```makefile
obj-m += hello.o
```
#### The kernel build system requires a Makefile to know which files to compile. obj-m tells the build system to create a loadable module (.ko file) from hello.c.

---

### ✅ [2/5] Compile Module

We need to compile against the exact kernel source tree and use ARM64 cross-compiler because our target system runs on ARM64.the purpose is to generate a .ko (kernel object) file that can be loaded into the running kernel.

```bash
make -C ../linux ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(pwd) modules
```
---

### ✅ [3/5] Copy Modele to rootfs.img

Mount the root filesystem image and install the compiled module in the appropriate kernel modules directory.

```bash
sudo mount -o loop ../rootfs.img /tmp/mnt
sudo mkdir -p /tmp/mnt/lib/modules/6.15.0/
sudo cp hello.ko /tmp/mnt/lib/modules/6.15.0/
sudo umount /tmp/mnt
```

> Wanna auto-mount while booting，can add upon command in `rcS` ：  
> `insmod /lib/modules/6.15.0/hello.ko`

---

### ✅ [4/5] Activate QEMU again（same command）

Launch the virtual Linux system with the updated root filesystem containing the extra kernel module.

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
---

### ✅ [5/5] Test whether Module is Mount

Load the kernel module and verify whether it works by checking kernel log messages.

Type in QEMU：

```sh
insmod /lib/modules/6.15.0/hello.ko
dmesg | tail
```

Output：

```
👋 Hello from kernel module!
```
---
