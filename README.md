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

```

# **📝 PA2_A - File I/O Operations in C**
## **📌 Overview**

This project is part of Programming Assignment 2 (PA2_A) for CSCI 460 - Operating Systems.
It demonstrates basic file I/O operations in C, allowing users to interact with a file through an interactive command-line interface.
🔹 Features

✅ Read from a file (r) – Read a specified number of bytes and display them.

✅ Write to a file (w) – Overwrite or add new content to the file.

✅ Seek in a file (s) – Move the file pointer to a specific position for reading or writing.

✅ Exit program (CTRL+D) – Close the application cleanly.

# **🚀 How to Compile & Run**
## **1️⃣ Compile the Program**

```
gcc -o pa2test pa2test.c
```

## **2️⃣ Run the Program**

```
./pa2test <filename>
```

## **📌 If the file does not exist, it will be created automatically.**

## **🛠 Example Usage**

### **1️⃣ Writing to a File**

**Option (r for read, w for write, s for seek)**: w

**Enter the string you want to write**: Hello, world!

### **2️⃣ Reading from a File**

**Option (r for read, w for write, s for seek)**: r

**Enter the number of bytes you want to read**: 5

**Read data**: Hello

### **3️⃣ Seeking in a File**

**Option (r for read, w for write, s for seek)**: s

**Enter an offset value**: 2

**Enter a value for whence (0 for SEEK_SET, 1 for SEEK_CUR, 2 for SEEK_END)**: 0

**New file position**: 2

### **4️⃣ Exiting the Program**

CTRL+D detected, exiting...

## **📜 Notes**

    The program provides error handling for invalid inputs and file operations.
    All operations modify the file permanently, so use with caution.
    Future improvements could include appending, clearing file contents, and file info retrieval.

# **📌 Author**
> Nate Heim




