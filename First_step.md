## 🔧 First step：Building Minimal Linux Environment

### ✅ [1/6] Install Development Tools

Install essential build tools and dependencies for compiling the Linux kernel, BusyBox, U-Boot, and running QEMU ARM64 simulation.

```bash
sudo apt install build-essential git qemu-system-aarch64 gcc-aarch64-linux-gnu libncurses-dev flex bison device-tree-compiler libgnutls28-dev wget
```
---
### ✅ [2/6] Build Busybox Rootfs

Download and build BusyBox to create a minimal root filesystem. BusyBox provides common Linux commands like ls, sh, mount.

```bash
wget -nc https://busybox.net/downloads/busybox-1.36.1.tar.bz2
tar -xf busybox-1.36.1.tar.bz2
cd busybox-1.36.1
make defconfig
sed -i 's/# CONFIG_STATIC is not set/CONFIG_STATIC=y/' .config
make -j$(nproc) CROSS_COMPILE=aarch64-linux-gnu-
make CONFIG_PREFIX=../rootfs install
```

Create operation bash(boot need) `/etc/init.d/rcS`：

```sh
#!/bin/sh
mount -t proc none /proc          #mount sys functions
mount -t sysfs none /sys          #mount hardware and driver info
exec /bin/sh                      #replace init process to avoid kernel panic
```
---

### ✅ [3/6] Create rootfs.img

Create an being used ext2 image file as the root filesystem in QEMU. The contents of the BusyBox installation are copied into this image.

```bash
dd if=/dev/zero of=rootfs.img bs=1M count=64
mkfs.ext2 rootfs.img
sudo mount -o loop rootfs.img mnt
sudo cp -a rootfs/. mnt/
sudo umount mnt
```
##### Currently, there is a ext2 image containing a fully functional BusyBox-based minimal Linux root filesystem exist.
---

### ✅ [4/6] Build Linux kernel

I use the official Linux kernel source, configure it for ARM64 using the default configuration, and build the "Image" file which is needed for QEMU booting.

```bash
git clone --depth=1 https://github.com/torvalds/linux.git
cd linux
make ARCH=arm64 defconfig
make -j$(nproc) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image
```
---

### ✅ [5/6] Build U-Boot

U-Boot acts as the bootloader when we simulate ARM64 with QEMU. This step builds a minimal version of U-Boot.

```bash
git clone --depth=1 https://github.com/u-boot/u-boot.git
cd u-boot
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- qemu_arm64_defconfig
make -j$(nproc) ARCH=arm CROSS_COMPILE=aarch64-linux-gnu-
```
---

### ✅ [6/6] Boot with QEMU

Now that we have all components (kernel Image, rootfs.img, and U-Boot), we can launch QEMU.

```bash
qemu-system-aarch64 \
  -M virt \
  -cpu cortex-a53 \
  -m 512M \
  -nographic \
  -bios u-boot/u-boot.bin \
  -kernel linux/arch/arm64/boot/Image \
  -append "root=/dev/vda rw console=ttyAMA0 init=/init" \
  -drive if=virtio,format=raw,file=rootfs.img
```
##### If everything is correctly executed, the BubyBox shell prompt will appear, indicating the minimal Linux environment is running successfully.

---
