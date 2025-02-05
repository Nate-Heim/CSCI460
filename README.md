# **CSCI 460 - Operating Systems**
## **Programming Assignment 1 (PA1)**  
--- This repository contains the implementation of the following ---

**Programming Assignment 1 (PA1)** for CSCI 460 - Operating Systems. This was split into **two parts**:  

- **PA1_A**: Configuring GRUB, compiling a Linux kernel, and adding a simple system call (`helloworld`).  
- **PA1_B**: Adding a second system call (`csci_add`) and working with Loadable Kernel Modules (LKM).  

---

# **📌 PA1_A: Compiling a Custom Linux Kernel & Adding a System Call**
## **1️⃣ Overview**
PA1_A focuses on setting up and compiling a custom Linux kernel and adding a new system call named `helloworld`.  

### **2️⃣ Steps Completed**
✅ **Configuring GRUB** to allow booting from multiple kernel versions.  
✅ **Downloading & compiling the Linux kernel** (`linux-hwe-4.15.0`).  
✅ **Adding a system call (`sys_helloworld`)** that prints "hello world" to the kernel logs.  
✅ **Recompiling the kernel and rebooting into it.**  
✅ **Writing a test program (`test_helloworld.c`)** to call the new system call.  

### **3️⃣ Key Files**
📂 **Kernel System Call Files**
- `arch/x86/kernel/helloworld.c` *(Contains the system call implementation.)*  
- `arch/x86/kernel/Makefile` *(Updated to compile `helloworld.c`.)*  
- `arch/x86/entry/syscalls/syscall_64.tbl` *(Registers the new system call.)*  
- `include/linux/syscalls.h` *(Declares the system call in the header file.)*  

📂 **Test Program**
- `test_helloworld.c` *(User-space program that calls `sys_helloworld`.)*  

📂 **Logs**
- `/var/log/syslog` *(Kernel log verifying the system call was executed.)*  

---

# **📌 PA1_B: Adding a Second System Call & Loadable Kernel Module (LKM)**
## **1️⃣ Overview**
PA1_B builds upon PA1_A by adding a **second system call (`csci_add`)** and a **Loadable Kernel Module (`helloModule`)**.  

### **2️⃣ Steps Completed**
✅ **Adding the `csci_add` system call** to add two integers and return the result.  
✅ **Using `copy_from_user()` and `copy_to_user()`** to safely handle user-space memory.  
✅ **Writing a test program (`test_csci_add.c`)** to call the system call.  
✅ **Creating a Loadable Kernel Module (`helloModule.ko`).**  
✅ **Loading and unloading the LKM using `insmod` and `rmmod`.**  

### **3️⃣ Key Files**
📂 **Kernel System Call Files**
- `arch/x86/kernel/csci_add.c` *(System call implementation.)*  
- `arch/x86/kernel/Makefile` *(Updated to compile `csci_add.c`.)*  
- `arch/x86/entry/syscalls/syscall_64.tbl` *(Registers the system call.)*  
- `include/linux/syscalls.h` *(Declares the system call in the header file.)*  

📂 **Test Program**
- `test_csci_add.c` *(User-space program that calls `csci_add`.)*  

📂 **Loadable Kernel Module (LKM)**
- `home/kernel/modules/helloModule.c` *(LKM implementation.)*  
- `home/kernel/modules/Makefile` *(Makefile for building the LKM.)*  
- `home/kernel/modules/helloModule.ko` *(Compiled LKM.)*  

📂 **Logs**
- `/var/log/syslog` *(Kernel logs confirming execution of `csci_add` and `helloModule`.)*  

---

# **📌 Running the Tests for PA1_B**
## **System Call Tests**
To test the `csci_add` system call:
```bash
gcc -o test_csci_add test_csci_add.c
./test_csci_add
Expected output was:
csci_add: 10 + 20 = 30

Verified this with :
dmesg | tail

At the end of the log you should see this:
csci_add: Adding 10 and 20
csci_add: Result = 30

