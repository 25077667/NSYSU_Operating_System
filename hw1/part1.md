---
title: Homework 1 Part1
author: B073040047
date: 7/14/2020
---
###### tags:OS2020Spring

This homework in [Hackmd Link](https://hackmd.io/@25077667/os-hw1)
---

## Compile your kernel!

:::danger
If you are using GTX 10** in your computer. You might  get freezing at boot time.

You can folow some resource below to solve it .
* https://itsfoss.com/fix-ubuntu-freezing/
* https://medium.com/caesars-study-review-on-web-development/acpi-osi-linux-nomodeset%E6%98%AF%E4%BB%80%E9%BA%BC%E6%84%8F%E6%80%9D-%E5%8A%9F%E8%83%BD-42d8e2c444c3
* https://www.twblogs.net/a/5cc24c53bd9eee3aed78a7ef
:::

Ok, let's start!

:::info
I put all commands in the ` ` zone.
:::

1. Choose your Linux distro.:
    * **Recommand** you should split *over 40 GB* disk space to contain your linux distro.
    * [Ubuntu](http://www.ubuntu-tw.org/modules/tinyd0/) is the easiest! I assume you are using Ubuntu below.
    * Fedora, Gentoo, Arch are also common distro.
        * What is distro.?
            * A Linux distribution (often abbreviated as distro).
    * Ubuntu is a branch of Debian, Fedora is a branch of Red Hat.
        * You can get more info. from [Wikipedia](https://en.wikipedia.org/wiki/Linux_distribution).
2. Open the Terminal and type:
    * `sudo apt update; sudo apt install build-essential libncurses-dev bison flex libssl-dev libelf-dev liblz4-tool vim -y`
    * To install some essential tools
        * If you are using non-debian OS, change the **apt** to your package manager.
3. Choose your kernel version:
    * Take the **newest** for example.
    * Go to [kernel.org](https://www.kernel.org/) to download the newest **stable** version with ==[tarball]==
    * Dowload to *~/Download*
        * *~/* means your home directory (a.k.a. /home/\<YOUR NAME\>)
        
4. Unzip the kernel which you downloaded:
    * `tar -xvf ~/Download/linux-*.tar`
6. `cd` in to the folder you unziped:
    * `cd ~/Download/linux-*`
8. Copy current config to here:
    * `cp /boot/config-*-generic .config`
9. Exclude the debug info, or it will take even more time:
    * Type `make menuconfig`.  
    * Enter ==Kernel hacking==, then ==Compile-time checks and compiler options==, excluding ==Compile the kernel with debug info== and save it.  
10. You have 2 methods to edit the version info:
    * Makefile
        * `vim Makefile`
            * [How to use vim?](https://hackmd.io/@93i7xo2/HJFCxkVdV)
        * Find the ==EXTRAVERSION \=== and add `-<YOUR STUDENT ID>`
        * `:wq` to save and quit
    * menuconfig
        * Enter in the ==General setup==
        * Select the ==Local version== and type `-<YOUR STUDENT ID>`
        * Make the 'y' to `*` (means select)
        * Save and exit
    > [name=25077667] Murmur: Edit the Makefile is much more easier and needless to search keywords in lots of lists.
    >> Actually, Linus torvalds also uses 'Makefile' method. [Prove](https://github.com/torvalds/linux/commit/11ba468877bb23f28956a35e896356252d63c983)

8. Make!
    * `make -j $(nproc)`
        * The `$(nproc)` can get the number of CPUs in your computer, that is full speed to run it!

:::you might get error message: `No rule to make target 'debian/canonical-certs.pem', ...`, [try this](https://stackoverflow.com/questions/67670169/compiling-kernel-gives-error-no-rule-to-make-target-debian-certs-debian-uefi-ce):::
9. Make module install
    * `sudo make modules_install`

10. Install it to /boot
    * `sudo make install`
11. Update Grub2
    * `sudo update-initramfs -c -k <THE VERSION YOU COMPILED>`
        * Of course, it contains your "stuID" in the end. That you append on.
    * `sudo update-grub`

12. Reboot!
    * `reboot`

Done.

---

## More Tools:
### If you learn it, it will help you very much.
[GNU Linux dev](https://hackmd.io/@sysprog/gnu-linux-dev/)

### A picture to realize vim: [picture](http://blog.vgod.tw/wp-content/uploads/2009/12/vgod-vim-cheat-sheet-full.pdf)

### What is sudo?
![Imgur](https://imgur.com/RLHGMWJ.png)
[source](https://www.facebook.com/307979116481335/photos/a.316111539001426/572406666705244/?type=3&theater)
[wiki](https://zh.wikipedia.org/zh-tw/Sudo)

### About the /boot partition
* What is disk partitions?
    - Disks can be parted to many partition, each partitions are independent, for example, you can install different OS in different partition.
    - **Partition table type**
        There are two different type of partirion table.
        **MBR** and **GPT** 
        - **MBR**
            - Master Boot Record , it's an old type method with old **bios** machine
            - Limit
                - Max size of partition is **2.1TB**
                - It can only part **4 Primary Partition**, if you need more partition, you have to part **3P1E**, which **P** means **Primary Partition** and **E** means **Extension Partition**, then part many **Logic Partition** as you want in **Extension Partition**.
                - Max partition number : **4**
                    - p.s. : **P+E** must $\le$ 4 and **E** has to $\le$ 1. 
        - **GPT**
            - GUID Partition Table , it's a newer method with in **UEFI** machine.
            - Limit
                - Max size of partition is **18EB**
                - Max partition number : **128**  
* What is partitions' format and flag?
    - Partition format is the rule of how files be stored and fetched.
        - **NTFS**
            - New Technology File System, This is the most used format in **Windows NT OS(>= **Windows NT 3.1**)**.
            - Page size : **4KB**
            - Max file size : **256TB**
            - p.s. : If you want to access **NTFS** in other OS like **Linux**s , you have to via **ntfs-3g**.
        - **ext4**
            - Fourth extended file system , this is the newest file system in **Unix-like** system.
            - Page size : **4KB**
            - Max file size : **16TB**
            - Max partition cabability : **1EB**
        - **fat32**
            - File Allocation Table
            - Max file size : **4GB**
            - Max partition cabability : **8TB**
            - p.s. : **efi** partition should be format to **fat32**
            - 
    - Flag
        - In MBR format needs a ==Boot flag== to mention boot loader it is bootable.
            
* How to split your partitions by yourself?
    - There are may in **linux** tools like `fdisk`, `gparted`, `lsblk`, `cfdisk`, this examples is using `fdisk`
    - `fdisk -l`
        - It will print all yout storage devices , sata devices will be list like */dev/sdX* , M.2 pcie devices will be list like */dev/nvme...*.
    - `fdisk /dev/sdX`
        - select disk **sdX** which you want to modify. 
    - In `fdisk` press `p` to list partition 
    - In `fdisk` press `o` to create new dos partition table(MBR) , press `g` to create gpt partition(UEFI)
    #### MBR 
    - part at least one partition which mount as `/`**(root)**
    - In `fdisk` press `n` to create a new partition
    - In `fdisk` press `t` to modify partition type(linux file system)
    #### GPT
    - recommend to part at least two partitions , one that mount as `/`**(root)** , the other mount as `/boot`
    - In `fdisk` press `n` to create a new partition
    - In `fdisk` press `t` to modify partition type(`/` : linux file system , `/boot` : efi)
    - formatting you partition
        - `mkfs.ext4 /dev/sdaXY`
            - format partition Y on disk X to ext4
        - `mkfs.vfat /dev/sdaXY`
            - format partition Y on disk X to fat32

* Concluding:
    * You can only split fewer space to finish this homework in older version of linux.
