# **Kernel Monitor Project**

This project consists of a Linux kernel module and a user-space application to gather and display kernel internals data (e.g., CPU usage, memory statistics, task details). The steps below are for me to re-run the project on my personal machine.

---

## **Prerequisites**

1. **Host Machine:**
   - Linux-based OS (e.g., Ubuntu).
   - QEMU installed.
   - ARM cross-compiler toolchain installed.
   - Kernel source code and root filesystem (`rootfs.ext4`) prepared.

2. **Files Provided:**
   - `kernel_monitor.ko`: The kernel module.
   - `monitor_app`: The user-space application.
   - `rootfs.ext4`: The root filesystem for QEMU.
   - `zImage`: The Linux kernel image.
   - `vexpress-v2p-ca9.dtb`: The device tree blob for the `vexpress-a9` machine.

---

## **Steps to Run the Project**

### **1. Prepare the Root Filesystem**

1. **Mount the Root Filesystem:**
   Mount the `rootfs.ext4` file to a directory (e.g., `/mnt/rootfs`) to copy files into it:

   ```bash
   #I already have the rootfs.ext4 there
   cd /home/galaxy/Desktop/EmbeddedLinux/Linux-yocto-Excersises/linux/code/linux 
   #mount the rootfs.ext4 to a folder :/mnt/rootfs
   sudo mount rootfs.ext4 /mnt/rootfs
   #(First time) Copy the precompiled file system to the mount point(i have the directory in the linux folder):
   cp rootfs/* /mnt/rootfs
   ```
   
2. **Build the module and the User interface:**
   build kernel_monitor.c && monitor_app.c:

   ```bash
   cd /home/galaxy/Desktop/kernel_monitor_project
   #clean made files (For demonstration purpose)
   make clean
   make -f Makefile.app clean
   #make them again
   make -C /home/galaxy/Desktop/EmbeddedLinux/Linux-yocto-Excersises/linux/code/linux M=$(pwd) modules #this produces the .ko module kernel_monitor.ko
   make -f Makefile.app #that produces monitor_app
   ```
   
3. 

2. **Copy Files into the Root Filesystem:**
   Copy the kernel module and user-space application into the mounted root filesystem:

   ```bash
   cd /home/galaxy/Desktop/kernel_monitor_project
   #create a directory to store the module
   mkdir /mnt/rootfs/projectModules
   #delete everything in the directory
   sudo rm * /mnt/rootfs/projectModules
   #copy the generated files to the created directory
   sudo cp kernel_monitor.ko /mnt/rootfs/home/
   sudo cp monitor_app /mnt/rootfs/home/
   ```
   
3. **Unmount the Root Filesystem:**
   Unmount the root filesystem after copying the files:

   ```bash
   sudo umount /mnt/rootfs
   ```

---

### **2. Start QEMU with the Linux Kernel and Root Filesystem**

Run the following command on your host machine to start the QEMU emulator with the ARM `vexpress-a9` machine:

```bash
#Run the QEMU Machine
sudo qemu-system-arm -M vexpress-a9 -m 128M -nographic \
-kernel /home/galaxy/Desktop/EmbeddedLinux/Linux-yocto-Excersises/linux/code/linux/arch/arm/boot/zImage \
-append "console=ttyAMA0 root=/dev/vda rw" \
-dtb /home/galaxy/Desktop/EmbeddedLinux/Linux-yocto-Excersises/linux/code/linux/arch/arm/boot/dts/arm/vexpress-v2p-ca9.dtb \
-drive if=none,file=rootfs.ext4,format=raw,id=hd0 \
-device virtio-blk-device,drive=hd0
```

This command:
- Uses the `vexpress-a9` machine type.
- Allocates 128MB of RAM.
- Boots the Linux kernel (`zImage`) located at the specified path.
- Uses the provided device tree blob (`dtb`).
- Mounts the root filesystem (`rootfs.ext4`) as `/dev/vda`.

Wait for the system to boot and log in as `root`.

---

### **3. Insert the Kernel Module**

In the QEMU terminal, navigate to the directory where the files are located and insert the kernel module:

```bash
#go to the file in which i stored the module
cd /projectModules
#load the module
insmod kernel_monitor.ko
```

Verify that the module is loaded:

```bash
lsmod | grep kernel_monitor
```

Check the kernel logs to confirm successful initialization:

```bash
dmesg | grep "Kernel Monitor"
```

---

### **4. Run the User-Space Application**

Run the `monitor_app` to display the kernel monitor data:

```bash
#Run the user interface
./monitor_app
```

Expected output:

```
Kernel Monitor Data:
CPU Usage: 230000000
Memory Usage: 26002
Task: init, PID: 1, Memory: 574
Task: sh, PID: 63, Memory: 574
Task: monitor_app, PID: 68, Memory: 343
```

---

### **5. Remove the Kernel Module (Optional)**

When done, remove the kernel module:

```bash
rmmod kernel_monitor
```

Verify that the module is removed:

```bash
lsmod | grep kernel_monitor
```



---

## **Files and Their Roles**

1. **`kernel_monitor.ko`:**
   - The kernel module that gathers kernel internals data and exposes it via `/proc/kernel_monitor`.

2. **`monitor_app`:**
   - The user-space application that reads and displays the data from `/proc/kernel_monitor`.

3. **`rootfs.ext4`:**
   - The root filesystem used by QEMU.

4. **`zImage` and `vexpress-v2p-ca9.dtb`:**
   - The Linux kernel image and device tree blob for the `vexpress-a9` machine.

---

## **Conclusion**

This project demonstrates how to create a Linux kernel module to gather kernel internals data and expose it to a user-space application.

