# **Kernel Monitor Project**

This project consists of a Linux kernel module and a user-space application to gather and display kernel internals data (e.g., CPU usage, memory statistics, task details). Below are the steps to clone, set up, and run the project.

---

## **Prerequisites**

1. **Host Machine:**
   - Linux-based OS (e.g., Ubuntu).
   - QEMU installed.
   - ARM cross-compiler toolchain installed.

2. **Files Provided in the Repository:**
   - `kernel_monitor.c`: The kernel module source code.
   - `monitor_app.c`: The user-space application source code.
   - `Makefile`: For building the kernel module.
   - `Makefile.app`: For building the user-space application.

---

## **Steps to Run the Project**

### **1. Clone the Repository**

Clone the repository to your local machine:

```bash
git clone https://github.com/galaxythereal/Linux-Kernel-Monitor.git
cd Linux-Kernel-Monitor
```

---

### **2. Install Required Libraries**

Install the necessary libraries for building the Linux kernel:

```bash
sudo apt install libelf-dev libssl-dev libgmp3-dev libmpc-dev
```

---

### **3. Build the Linux Kernel**

1. **Download and Checkout the Linux Kernel:**

   ```bash
   git clone git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
   cd linux
   git checkout linux-6.7.y
   ```

2. **Set Up the Environment for Cross-Compilation:**

   ```bash
   export PATH=/path/to/arm-gnu-toolchain-13.3.rel1-x86_64-arm-none-linux-gnueabihf/bin:$PATH
   export CROSS_COMPILE=arm-none-linux-gnueabihf-
   export ARCH=arm
   ```

3. **Configure and Build the Kernel:**

   ```bash
   make vexpress_defconfig
   make menuconfig  # Enable "Networking support -> Networking options -> 802.1Q/802.ad VLAN Support <M>"
   make            # Build the kernel
   make modules    # Build the kernel modules
   ```

4. **Install the Kernel Modules:**

   ```bash
   make INSTALL_MOD_PATH=/srv/nfs4/nfsroot modules_install
   ```

---

### **4. Set Up the Root Filesystem**

1. **Download the Root Filesystem:**

   You can use BusyBox or any other precompiled fs, i use one which i refer to by : rootfs and i have it stored in my linux directory

2. **Create and Prepare the Root Filesystem:**

   ```bash
   # Create a 1GB file for the root filesystem
   dd if=/dev/zero of=rootfs.ext4 bs=1M count=1024
   mkfs.ext4 rootfs.ext4
   
   # Mount the root filesystem and copy the contents
   sudo mount rootfs.ext4 /mnt
   sudo cp -a rootfs/* /mnt/
   sudo umount /mnt
   ```

---

### **5. Build the Kernel Module and User-Space Application**

1. **Build the Kernel Module:**

   ```bash
   cd /path/to/linux/repository
   make -C /path/to/linux/source M=$(pwd) modules
   ```

2. **Build the User-Space Application:**

   ```bash
   make -f Makefile.app
   ```

---

### **6. Prepare the Root Filesystem for QEMU**

1. **Mount the Root Filesystem:**

   ```bash
   sudo mount rootfs.ext4 /mnt/rootfs
   ```

2. **Copy the Kernel Module and User-Space Application:**

   ```bash
   sudo mkdir -p /mnt/rootfs/projectModules
   sudo cp kernel_monitor.ko /mnt/rootfs/projectModules/
   sudo cp monitor_app /mnt/rootfs/projectModules/
   ```

3. **Unmount the Root Filesystem:**

   ```bash
   sudo umount /mnt/rootfs
   ```

---

### **7. Start QEMU with the Linux Kernel and Root Filesystem**

Run the following command to start the QEMU emulator with the ARM `vexpress-a9` machine:

```bash
sudo qemu-system-arm -M vexpress-a9 -m 128M -nographic \
-kernel /path/to/linux/arch/arm/boot/zImage \
-append "console=ttyAMA0 root=/dev/vda rw" \
-dtb /path/to/linux/arch/arm/boot/dts/vexpress-v2p-ca9.dtb \
-drive if=none,file=rootfs.ext4,format=raw,id=hd0 \
-device virtio-blk-device,drive=hd0
```

Wait for the system to boot and log in as `root`.

---

### **8. Insert the Kernel Module**

In the QEMU terminal, navigate to the directory where the files are located and insert the kernel module:

```bash
cd /projectModules
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

### **9. Run the User-Space Application**

Run the `monitor_app` to display the kernel monitor data:

```bash
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

### **10. Remove the Kernel Module (Optional)**

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

1. **`kernel_monitor.c`:**
   - The source code for the kernel module that gathers kernel internals data and exposes it via `/proc/kernel_monitor`.

2. **`monitor_app.c`:**
   - The source code for the user-space application that reads and displays the data from `/proc/kernel_monitor`.

3. **`Makefile`:**
   - Builds the kernel module.

4. **`Makefile.app`:**
   - Builds the user-space application.

5. **`rootfs.ext4`:**
   - The root filesystem used by QEMU.

6. **`zImage` and `vexpress-v2p-ca9.dtb`:**
   - The Linux kernel image and device tree blob for the `vexpress-a9` machine.

---

## **Conclusion**

This project demonstrates how to create a Linux kernel module to gather kernel internals data and expose it to a user-space application. Follow the steps above to set up and run the project from scratch.
# Kernel_Monitor
