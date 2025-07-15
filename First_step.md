## 🔧 First step：Building Minimal Linux Environment

### ✅ [1/6] Install Development Tools

```bash
sudo apt install build-essential git qemu-system-aarch64 gcc-aarch64-linux-gnu libncurses-dev flex bison device-tree-compiler libgnutls28-dev wget
```
---
### ✅ [2/6] Build Busybox Rootfs

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

### ✅ [3/6] Build rootfs.img

```bash
dd if=/dev/zero of=rootfs.img bs=1M count=64
mkfs.ext2 rootfs.img
sudo mount -o loop rootfs.img mnt
sudo cp -a rootfs/. mnt/
sudo umount mnt
```
---

### ✅ [4/6] Compile Linux kernel

```bash
git clone --depth=1 https://github.com/torvalds/linux.git
cd linux
make ARCH=arm64 defconfig
make -j$(nproc) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image
```
---

### ✅ [5/6] Compile U-Boot

```bash
git clone --depth=1 https://github.com/u-boot/u-boot.git
cd u-boot
make ARCH=arm CROSS_COMPILE=aarch64-linux-gnu- qemu_arm64_defconfig
make -j$(nproc) ARCH=arm CROSS_COMPILE=aarch64-linux-gnu-
```
---

### ✅ [6/6] Activate QEMU

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
---
