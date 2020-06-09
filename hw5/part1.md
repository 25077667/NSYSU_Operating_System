---
title: Homework 5 Part1
author: B073040047
date: 6/9/2020
---
###### tags:OS2020Spring

This homework in [Hackmd Link](https://hackmd.io/@25077667/B13Iya23I)
---

## 1. What is the difference between a hard link and a symbolic link? Give an advantage of each one.

* symbolic link:
    * Description:
        * A symbolic link is term for any file that contains a reference to another file or directory in the form of an absolute or relative path and that affects pathname resolution.
        * A symbolic link contains a text string that is automatically interpreted and followed by the operating system as a path to another file or directory.
        * Like this:
            * ![Imgur](https://imgur.com/eU5wA8r.png)
    * Advantage:
        * It is usually more widely used than the hard links. It could cross over different file systems.
* hard link:
    * The hard link is a directory entry that associates a name with a file on a file system. All directory-based file systems must have at least one hard link giving the original name for each file. The term “hard link” is usually only used in file systems that allow more than one hard link for the same file.
    * Like this:
        * ![Imgur](https://i.imgur.com/iomxTos.png)
    * Adventage:
        * The main advantage of hard links is that, compared to soft links, there is no size or speed penalty. Soft links are an extra layer of indirection on top of normal file access; the kernel has to dereference the link when you open the file, and this takes a small amount of time. The link also takes a small amount of space on the disk, to hold the text of the link. These penalties do not exist with hard links because they are built into the very structure of the filesystem.
* Demo:
    * We can see the inode number to guess how it works:
    * ![Imgur](https://i.imgur.com/uk0f5iL.png)

## 2. A disk has 4000 cylinders, each with 8 tracks of 512 blocks. A seek takes 1 msec per cylinder moved. If no attempt is made to put the blocks of a file close to each other, two blocks that are logically consecutive (i.e., follow one another in the file) will require an average seek, which takes 5 msec. If, however, the operating system makes an attempt to cluster related blocks, the mean interblock distance can be reduced to 2 cylinders and the seek time reduced to 100 microsec. How long does it take to read a 100 block file in both cases, if the rotational latency is 10 msec and the transfer time is 20 microsec per block?

Suppose the 100 blocks are not consecutive, and the expectation of taking time is average time.
Then,

* For **non-adjacent** blocks of file to each other:
    * $(5 + 10 + 0.02) \cdot 100 = 1502$ ms
* For adjacent blocks of file to each other:
    * $(0.1 + 10 + 0.02) \cdot 100 = 1012$ ms


## 3. Consider the following page reference string:
```
1, 2, 3, 4, 2, 1, 5, 6, 2, 1, 2, 3, 7, 6, 3, 2, 1, 2, 3, 6.
```
How many page faults would occur for the following replacement algorithms, assuming one, two, three,
four, five, six, or seven frames? Remember all frames are initially empty, so your first unique pages will all
cost one fault each.

(a) LRU replacement:
1. 20
2. 18
3. 15
4. 10
5. 8
6. 7
7. 7

And here is the LRU code in C++
```cpp=
// We can use stl container list as a double
// ended queue to store the cache keys, with
// the descending time of reference from front
// to back and a set container to check presence
// of a key. But to fetch the address of the key
// in the list using find(), it takes O(N) time.
// This can be optimized by storing a reference
//	 (iterator) to each key in a hash map.

#include <bits/stdc++.h>
#include <boost/range/irange.hpp>
using namespace std;

class LRUCache
{
    // store keys of cache
    list<int> dq;

    // store references of key in cache
    unordered_map<int, list<int>::iterator> ma;
    int csize;  // maximum capacity of cache

public:
    LRUCache(int);
    void refer(int);
    void display();
    int fault;
};

// Declare the size
LRUCache::LRUCache(int size)
{
    this->csize = size;
    this->fault = 0;
}

// Refers key x with in the LRU cache
void LRUCache::refer(int x)
{
    // not present in cache
    if (ma.find(x) == ma.end()) {
        // cache is full
        if (dq.size() == csize) {
            // delete least recently used element
            int last = dq.back();
            // Pops the last elmeent
            dq.pop_back();
            // Erase the last
            ma.erase(last);
        }
        fault++;
    }

    // present in cache
    else
        dq.erase(ma[x]);

    // update reference
    dq.push_front(x);
    ma[x] = dq.begin();
}

// Function to display contents of cache
void LRUCache::display()
{
    // Iterate in the deque and print
    // all the elements in it
    for (auto it : dq)
        cout << it << " ";

    cout << endl;
}

int main()
{
    vector<int> v = {1, 2, 3, 4, 2, 1, 5, 6, 2, 1,
                     2, 3, 7, 6, 3, 2, 1, 2, 3, 6};
    for (auto i : boost::irange(1, 8)) {
        LRUCache ca(i);
        for (auto j : v)
            ca.refer(j);
        ca.display();
        cout << "Falut times: " << ca.fault << endl;
    }
    return 0;
}
// This code is contributed by Satish Srinivas
// Modified by 25077667
```

(b) Optimal replacement
1. 20
2. 15
3. 11
4. 8
5. 7
6. 7
7. 7

And here is the Optimal replacement code in C++
```cpp=
#include <bits/stdc++.h>
using namespace std;

int predict(int page[], vector<int> &fr, int pn, int index)
{
    // Store the index of pages which are going
    // to be used recently in future
    int res = -1, farthest = index;
    for (int i = 0; i < fr.size(); i++) {
        int j;
        for (j = index; j < pn; j++) {
            if (fr[i] == page[j]) {
                if (j > farthest) {
                    farthest = j;
                    res = i;
                }
                break;
            }
        }
        // Return the page which are
        // are never referenced in future,
        if (j == pn)
            return i;
    }
    // If all of the frames were not in future,
    // return any of them, we return 0. Otherwise
    // we return res.
    return (res == -1) ? 0 : res;
}
bool search(int key, vector<int> &fr)
{
    for (int i = 0; i < fr.size(); i++)
        if (fr[i] == key)
            return true;
    return false;
}

void opr(int page[], int pn, int fn)
{
    vector<int> fr;
    int hit = 0;
    for (int i = 0; i < pn; i++) {
        // Page found in a frame : HIT
        if (search(page[i], fr)) {
            hit++;
            continue;
        }
        // If a page not found in a frame : MISS
        // check if there is space available in frames.
        if (fr.size() < fn)
            fr.push_back(page[i]);
        // Find the page to be replaced.
        else {
            int j = predict(page, fr, pn, i + 1);
            fr[j] = page[i];
        }
    }
    cout << "Hits = " << hit << endl;
    cout << "Misses = " << pn - hit << endl;
}

int main()
{
    int page[] = {1, 2, 3, 4, 2, 1, 5, 6, 2, 1, 2, 3, 7, 6, 3, 2, 1, 2, 3, 6};
    int pn = sizeof(page) / sizeof(page[0]);
    for (int i = 1; i < 8; i++)
        opr(page, pn, i);
    return 0;
}
```

## 4. Consider a file currently consisting of 100 blocks. Assume that the file control block (and the index block, in the case of indexed allocation) is already in memory. Calculate how many disk I/O operations are required for contiguous, linked, and indexed (single-level) allocation strategies, if, for one block, the following conditions hold. In the contiguous allocation case, assume that there is no room to grow in the beginning, but there is room to grow in the end. Assume that the block information to be added is stored in memory.





|     | Contiguous | Linked | Indexed |
|:---:|:----------:|:------:|:-------:|
|  a  |    201     |   1    |    1    |
|  b  |    101     |   52   |    1    |
|  c  |     1      |   3    |    1    |
|  d  |    198     |   1    |    0    |
|  e  |     98     |   52   |    0    |

* Explain:
    * (a) The block is added at the beginning.
        * For contiguous allocation strategies, we need 100 disk I/Os for reading the entire file into memory, 1 disk I/O for writing the new block to disk at the beginning of the file ( in the 1st block), and 100 disk I/Os for writing the remaining 100 blocks of the file to disk starting from the 2nd block. In total, we need 201 disk I/Os.
        * For linked allocation strategies, we need to change the pointer in the file control block to point to the new block in disk. In total, we need 1 disk I/Os.
        * For indexed allocation strategies, we write the new block to disk, and update indexed in memory.
    * (b) The block is added in the middle.
        * For contiguous allocation strategies, we need 50 disk I/Os for reading the last half of the file into memory, 1 disk I/O for writing the new block to disk in the middle of the file (in the 51st block), and 50 disk I/Os for writing the remaining 50 blocks of the file to disk starting from the 52nd block, and 1 disk I/O for writing the file control block to change the file length to 100. In total, we need 101 disk I/Os.
        * For linked allocation strategies, we read first 50 blocks, modify the point of 50th block, write new block to   first block. In total, we need 52 disk I/Os.
        * For indexed allocation strategies, we write the new block to disk, and update indexed in memory.
    * \(c\) The block is added at the end.
        * For contiguous allocation strategies, we write block in to the end of file. Hence, we need 1 disk I/Os.
        * For linked allocation strategies, we read the last block(using the point of end block), and then modify it, write new block to first block. In total, we need 3 disk I/Os.
        * For indexed allocation strategies, we write the new block to disk, and update indexed in memory.
    * (d) The block is removed from the beginning.
        * For contiguous allocation strategies, we need 99 disk I/Os for reading the last 99 blocks into memory, and 99 disk I/Os for writing the 99 blocks back to disk at the beginning of the file. In total, we need 198 disk I/Os.
        * For linked allocation strategies, we read the first block and copy it's point to the point of beginning(copying is in the memory). In total, we need 1 disk I/Os.
        * For indexed allocation strategies, we update indexed in memory.
    * (e) The block is removed from the middle.
        * For contiguous allocation strategies, we need 49 disk I/Os for reading the last 49 blocks into memory, and 49 disk I/Os for writing the 49 blocks back to disk starting from the 51th block. In total, we need 98 disk I/Os.
        * For linked allocation strategies, we read the first 51 block and copy the point of 51th to the point of 50th. In total, we need 52 disk I/Os.
        * For indexed allocation strategies, we update indexed in memory.

* Consider more:
    * What if the block is **removed** from the **end**?
        * For contiguous allocation strategies, we only need to update size in memory. In total, we need 0 disk I/Os.
        * For linked allocation strategies, we read the first 99 block and copy the point of end to the point of 99th. In total, we need 100 disk I/Os.
        * For indexed allocation strategies, we also update indexed in memory. Also 0 I/Os.


## 5. A certain computer provides its users with a virtual-memory space of 2 32 bytes. The computer has 2 18 bytes of physical memory. The virtual memory is implemented by paging, and the page size is 4096 bytes. A user process generates the virtual address 11123456. Explain how the system establishes the corresponding physical location. Distinguish between software and hardware operations.

:::warning
I guess that 11123456 means "0x11123456".
But we usually used non-prefix to be decimal.
:::

* Take 11123456 as "0x11123456":
    * The virtual address in binary form is
		`0001 0001 0001 0010 0011 0100 0101 0110`
        Since the page size is $2^{12}$, the page table size is $2^{20}$. Therefore, the low-order 12 bits (0100 0101 0110) are used as the displacement into the page, while the remaining 20 bits (0001 0001 0001 0010 0011) are used as the displacement in the page table.

* Take 11123456 as "11123456"(decimal):
    * The virtual address in binary form is
		`0000 0000 1010 1001 1011 1011 0000 0000`
        Since the page size is $2^{12}$, the page table size is $2^{20}$. Therefore, the low-order 12 bits (1011 0000 0000) are used as the displacement into the page, while the remaining 20 bits (0000 0000 1010 1001 1011) are used as the displacement in the page table.
