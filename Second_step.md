## 👋 Second Step：Build and Test Hello Kernel Module

### ✅ [1/5] Create Module Source Code  `hello.c`

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

```makefile
obj-m += hello.o
```
---

### ✅ [2/5] Compile Module

```bash
make -C ../linux ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(pwd) modules
```
---

### ✅ [3/5] Copy Modele to rootfs.img

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
