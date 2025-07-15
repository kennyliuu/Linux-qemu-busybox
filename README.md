# 🐧 Linux-qemu-busybox
In order to realize and learn the Linux kernel, it's a little side project about building and simulating a minimal Linux(for ARM64 with BusyBox and QEMU), include kernel module and debugfs.

## 📁 Side Project Structure

```text
minimal-linux/
├── busybox-1.36.1/
├── hello_module/
│   ├── hello.c
│   └── Makefile
├── linux/                  # Linux kernel
├── u-boot/                 # U-Boot bootloader
├── rootfs.img              # minimal root filesystem
```
- 👉 First_step.md: set up environment

- 👉 Second_step.md: implement mounting module

- 👉 Third_step.md: add debugfs
