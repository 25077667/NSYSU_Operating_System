OS final solutions
===
Contributed by < `25077667` >
Date: 2020/06/17
Creative Commons [CC0](https://creativecommons.org/publicdomain/zero/1.0/deed.zh_TW)

This markdown in [Hackmd Link](https://hackmd.io/@25077667/BJ1G_vw68)
---

==2018==
1. 200*200 array initializing, column major, each element size 8, page size 200, 3 page frames. Show how many **page faults**?
    * traverse by row
        ```
        for (int i = 0; i < 200; i++) 
            for(int j = 0; j < 200; j++)
                A[i][j] = 0;
        ```
        * Each time(loop) loads a page in colum, but modifies in row.
            * 200 * 200 = 40000
    * traverse by column
        ```
        for (int j = 0; j < 200; j++) 
            for(int i = 0; i < 200; i++)
                A[i][j] = 0;
        ```
        * A page has 200 bytes, a double has 8 bytes. So, a page can contain 25 elements of array.
            * $\frac{200}{8} = 25$. 
        * A column needs 8 page to contain.
            * $\frac{200}{25} = 8$
        * Have 200 columns, each column needs 8 pages. Total needs 1600 pages.
            * $200 \cdot 8 = 1600$

2. If random seek for 6ms, if cluster seek for 0.2ms. 300 blocks. rotate 3ms, transfer 0.01ms
    * Need seek and rotate and transfer.
    * Random
        * $(6 + 3 + 0.01) \cdot 300 = 2703ms$
    * Clustered
        * $(0.2 \cdot 2 + 3 + 0.01) \cdot 300 = 1023ms$

3. 8G, unit 4K. for bit map size
    * How many units to point all RAM?
        * $\frac{8\ GB}{4\ KB} = 2^{21}$
    * $\frac{2^{21}}{8}= 2^{18}$ Bytes = $2^8$ KB

4. String's length n, p distinct page
    * lower bound: (load all pages)
        * p
    * uppper boung: (each byte need a page to load in)
        * n

5. 48-bit virtual. 4KB page size.
    * $\frac{2^{48}}{2^{12}} = 2^{36}$

6. file size
    * smallest
        * 0, because it can point to nothing.
    * biggest
        * A pointer can point to 8K, the efficient will extend to 1024 times.
            * 8K/8 = 1K
        * (1K * 10 + 1M + 1G + 1T) * 8 Bytes

7. disk scheduling(125->200)
    * SCAN(go to last block of disk, then service back to begin block of disk)
        * 1848
            * 200 -> 250 -> 350 -> 400 -> 500 -> 550 -> 600 -> 800 -> 999 -> 100 -> 50 -> 0
            * Total distance: 50 + 100 + 50 + 100 + 50 + 50 + 200 + 199 + 899 + 50 + 50 = 1798
    * LOOK(go to biggest/samllest request, then service back)
        * 1350
            * 200 -> 250 -> 350 -> 400 -> 500 -> 550 -> 600 -> 800 -> 100 -> 50
            * Total distance: 50 + 100 + 50 + 100 + 50 + 50 + 200 + 700 + 50 = 1350
    * C-SCAN(not service back)
        * 1898
            * 200 -> 250 -> 350 -> 400 -> 500 -> 550 -> 600 -> 800 -> 999 -> 0 -> 50 -> 100
            * Total distance: 50 + 100 + 50 + 100 + 50 + 50 + 200 + 199 + 999 + 50 + 50 = 1898
    * C-LOOK(not servece back)
        * 1400
            * 200 -> 250 -> 350 -> 400 -> 500 -> 550 -> 600 -> 800 -> 50 -> 100
            * Total distance: 50 + 100 + 50 + 100 + 50 + 50 + 200 + 750 + 50 = 1400
    * SSTF(shortest seek first)
        * 1350 
            * 200 -> 250 -> 350 -> 400 -> 500 -> 550 -> 600 -> 800 -> 100 -> 50
            * Total distance: 50 + 100 + 50 + 100 + 50 + 50 + 200 + 700 + 50 = 1350

8. Paging faults of different algorithms
* 1, 2, 3, 4, 2, 1, 5, 6, 2, 1, 2, 3, 7, 6, 3, 2, 1, 2, 3, 6
    * LRU
        * 3 frames
            * 15 times
            ```
            [ 1 ]F
            [ 2 1 ]F
            [ 3 2 1 ]F
            [ 4 3 2 ]F
            [ 2 4 3 ]
            [ 1 2 4 ]F
            [ 5 1 2 ]F
            [ 6 5 1 ]F
            [ 2 6 5 ]F
            [ 1 2 6 ]F
            [ 2 1 6 ]
            [ 3 2 1 ]F
            [ 7 3 2 ]F
            [ 6 7 3 ]F
            [ 3 6 7 ]
            [ 2 3 6 ]F
            [ 1 2 3 ]F
            [ 2 1 3 ]
            [ 3 2 1 ]
            [ 6 3 2 ]F
            ```
        * 4 frames
            * 10 times
            ```
            [ 1 ]F
            [ 2 1 ]F
            [ 3 2 1 ]F
            [ 4 3 2 1 ]F
            [ 2 4 3 1 ]
            [ 1 2 4 3 ]
            [ 5 1 2 4 ]F
            [ 6 5 1 2 ]F
            [ 2 6 5 1 ]
            [ 1 2 6 5 ]
            [ 2 1 6 5 ]
            [ 3 2 1 6 ]F
            [ 7 3 2 1 ]F
            [ 6 7 3 2 ]F
            [ 3 6 7 2 ]
            [ 2 3 6 7 ]
            [ 1 2 3 6 ]F
            [ 2 1 3 6 ]
            [ 3 2 1 6 ]
            [ 6 3 2 1 ]
            ```
        * 5 frames
            * 8 times
            ```
            [ 1 ]F
            [ 2 1 ]F
            [ 3 2 1 ]F
            [ 4 3 2 1 ]F
            [ 2 4 3 1 ]
            [ 1 2 4 3 ]
            [ 5 1 2 4 3 ]F
            [ 6 5 1 2 4 ]F
            [ 2 6 5 1 4 ]
            [ 1 2 6 5 4 ]
            [ 2 1 6 5 4 ]
            [ 3 2 1 6 5 ]F
            [ 7 3 2 1 6 ]F
            [ 6 7 3 2 1 ]
            [ 3 6 7 2 1 ]
            [ 2 3 6 7 1 ]
            [ 1 2 3 6 7 ]
            [ 2 1 3 6 7 ]
            [ 3 2 1 6 7 ]
            [ 6 3 2 1 7 ]
            ```
    * Optimal
        * 3 frames
            * 11 times
            ```
            [ 1 ]F
            [ 1 2 ]F
            [ 1 2 3 ]F
            [ 1 2 4 ]F
            [ 1 2 4 ]
            [ 1 2 4 ]
            [ 1 2 5 ]F
            [ 1 2 6 ]F
            [ 1 2 6 ]
            [ 1 2 6 ]
            [ 1 2 6 ]
            [ 3 2 6 ]F
            [ 3 7 6 ]F
            [ 3 7 6 ]
            [ 3 7 6 ]
            [ 3 2 6 ]F
            [ 3 2 1 ]F
            [ 3 2 1 ]
            [ 3 2 1 ]
            [ 3 2 6 ]F
            ```
        * 4 frames
            * 8 times
                ```
                [ 1 ]F
                [ 1 2 ]F
                [ 1 2 3 ]F
                [ 1 2 3 4 ]F
                [ 1 2 3 4 ]
                [ 1 2 3 4 ]
                [ 1 2 3 5 ]F
                [ 1 2 3 6 ]F
                [ 1 2 3 6 ]
                [ 1 2 3 6 ]
                [ 1 2 3 6 ]
                [ 1 2 3 6 ]
                [ 7 2 3 6 ]F
                [ 7 2 3 6 ]
                [ 7 2 3 6 ]
                [ 7 2 3 6 ]
                [ 1 2 3 6 ]F
                [ 1 2 3 6 ]
                [ 1 2 3 6 ]
                [ 1 2 3 6 ]
                ```
        * 5 frames
            * 7 times
                * Try it yourself!

9. TLB reduce overhead
    * Treat it as expection, suppose hit rate(probability) is p
        * $100p + (500+100)(1-p) \le 200$
        * So, $p = \frac{4}{5}$

Other questions
---

* (2012.1)![Imgur](https://imgur.com/f7Ra0Kt.png)
    * No output.
        * Because the parent process get the IPC in STDIN buffer, but not recieve it in or process something out.


* (2014.4)A small computer has 8 page frames, each containing a page. The page frames contain virtual pages A, C, G, H, B, L, N, and D in that order. Their respective load times were 18, 23, 5, 7, 32, 19, 3, and 8. Their reference bits are 1, 0, 1, 1, 0, 1, 1, and 0 and their modified bits are 1, 1, 1, 0, 0, 0, 1, and 1, respectively. Which page will the second chance page replacement algorithm replace?
    * D.
        * Construct the FIFO queue first.
        * N -> G-> H -> D -> A -> L -> C -> B 
        * Consider the algorithm:
            ```md=
            Pages in list are sorted in FIFO order
            R bits are cleared regularly
            If the R bit of the oldest page is set it is put at the end of the list
            If all the pages in the list have been referenced the page that was “recycled” will reappear with the R bit cleared and will be thrown away.
            ```
        * Page 0, because the page 0 R bit is 0, and the second-earliest page loaded. The page 2’s R bit is 1, will put at the end of the list by FIFO replace scheduler.
        * Then we can find D.

* (2012.4)A computer has 6 tape drives, with n processes competing for it. Each process may need 2 drives. For which values of n is the system deadlock free?
    * $n \lt 6$
        * With three proccesses, each one can have two drives. With four processes, the distribution of drives will be (2,2,1,1), allowing the first two processes to finsh. With five processes, the distribution of drives will be (2,1,1,1,1). which still allows the first one to finish. With six processes, each holding one tape drive, and wanting another one, we have a deadlock. Thus for n < 6, the system is deadlock free.

* (2012.5)Disk requests come in to the driver for cylinders 10, 22, 20, 2, 40, 6, and 38, in that order. A seek takes 6 msec per cylinder moved. How much seek time is needed for
    * (a) Closest cylinder next, and
    * (b) Elevator algorithm (initially moving upward).
    * **--slpit line--**
    * Answer:
        * (a) 360ms
            * 20 -> 22 -> 10 -> 6 -> 2 -> 38 -> 40
            * $(2 + 12 + 4 + 4 + 36 + 2) \cdot 6 = 360ms$
        * (b) 348ms
            * 20 -> 22 -> 38 -> 40 -> 10 -> 6 -> 2
            * $(2 + 16 + 2 + 30 + 4 + 4) \cdot 6 = 348ms$


* (2016.6)![Imgur](https://imgur.com/QreCFx5.png)
    * (a) 10241
        * 8192 + 2049
            * Virtual address $2049 \in [0K, 4K)$ (the first page), which point to Frame 2.
            * Frame 2 address is $[8K, 12K)$. So, the basis of frame 2 is 8K.
            * From 8K offset $(2049 \equiv 2049 \mod 4096)$ is the answer.
    * (b) 24577
        * 24576 + 1
            * Virtual address $8193 \in [8K, 12K)$ (the third page), which point to Frame 6.
            * Frame 6 address is $[24K, 28K)$. So, the basis of frame 6 is 24K.
            * From 24K offset $(8193 \equiv 1 \mod 4096)$ is the answer.

* (2011.5)A small computer has four page frames. At the first clock tick, the R bits are 0111 (page 0 is 0, the rest are 1). At subsequent clock ticks, the values are 1011, 1010, 1101, 0010, 1010, 1100, 0001, 0101, 1011, and 1101. If the aging algorithm is used with a 5-bit counter, give the values of the four counters after the last ticks.
    * page 0: 11001
    * page 1: 10101
    * page 2: 01000
    * page 3: 11110
        * BJ4, Write it down, you can see it immediately.


* (2009.4)A computer has four page frames. The time of loading, time of last access, and the R and M bits for each page are as shown below (the times are in clock ticks):


| Page | Loaded | Last Ref. |  R  |  M  |
|:----:|:------:|:---------:|:---:|:---:|
|  0   |   126  |    279    |  0  |  0  |
|  1   |   230  |    260    |  1  |  0  |
|  2   |   120  |    272    |  1  |  1  |
|  3   |   160  |    280    |  1  |  1  |

* (a) Which page will NRU replace?
    * Page 0
        * Because it's RM = 0.

    * Explain page 17:
        * When process starts, R and M bit are set to 0
        * Periodically R bit is cleared to take into account for pages that have not
        been referenced recently
        * Pages are classified according to R and M
            * Class 0: not referenced, not modified
            * Class 1: not referenced, modified (R bit has been cleared!)
            * Class 2: referenced, not modified
            * Class 3: referenced, modified
        * NRU removes page at random from lowest numbered non empty class
        * Easy to implement but not terribly effective 

            So, the RM bits looks like:
            $$
            00 \ for \ R=0, M=0 \\
            01 \ for \ R=0, M=1 \\
            10 \ for \ R=1, M=0 \\
            11 \ for \ R=1, M=1
            $$

* (b) Which page will FIFO replace?
    * Page 2
        * Because the page 2 is the oldest loaded page.(in the FIFO queue)

* \(c\) Which page will LRU replace?
    * Page 1
        * Because the page 1's last reference time is 260, which is the oldest referenced time.

    * Explain:
        * The least recently used (LRU) page replacement algorithm, though similar in name to NRU, differs in the fact that LRU keeps track of page usage over a short period of time, while NRU just looks at the usage in the last clock interval. LRU works on the idea that pages that have been most heavily used in the past few instructions are most likely to be used heavily in the next few instructions too.

* (d) Which page will second chance replace?
    * Page 0
        * Because the page 0 R bit is 0, and the second-earliest page loaded. The page 2's R bit is 1, will put at the end of the list by FIFO replace scheduler.

    * Explain page 19:
        * Pages in list are sorted in FIFO order
        * R bits are cleared regularly
        * If the R bit of the oldest page is set it is put at the end of the list
        * If all the pages in the list have been referenced the page that was “recycled” will reappear with the R bit cleared and will be thrown away.



:::info
Source: 2018, 2016, 2014, 2013, 2012, 2011, 2010, 2009
:::
