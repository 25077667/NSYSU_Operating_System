---
title: Homework 4 Part1
author: B073040047
date: 5/28/2020
---
###### tags:OS2020Spring

This homework in [Hackmd Link](https://hackmd.io/@25077667/HJjWLCniU)
---

1. A computer has four page frames. The time of loading, time of last access, and the R and M bits for each page are as shown below (the times are in clock ticks):

| Page | Loaded | Last Ref. | R   | M   |
| ---- | ------ | --------- | --- | --- |
| 0    | 126    | 279       | 0   | 0   |
| 1    | 230    | 260       | 1   | 0   |
| 2    | 120    | 272       | 1   | 1   |
| 3    | 160    | 280       | 1   | 1   |


(a) Which page will NRU replace?
Page 0, because it's RM = 0.

* Explain[^first] page 17:
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

(b) Which page will FIFO replace?
Page 2, because the page 2 is the oldest loaded page.(in the FIFO queue)

\(c\) Which page will LRU replace?
Page 1, because the page 1's last reference time is 260, which is the oldest referenced time.

* Explain[^second]:
    * The least recently used (LRU) page replacement algorithm, though similar in name to NRU, differs in the fact that LRU keeps track of page usage over a short period of time, while NRU just looks at the usage in the last clock interval. LRU works on the idea that pages that have been most heavily used in the past few instructions are most likely to be used heavily in the next few instructions too.

(d) Which page will second chance replace?
Page 0, because the page 0 R bit is 0, and the second-earliest page loaded. The page 2's R bit is 1, will put at the end of the list by FIFO replace scheduler.

* Explain[^first] page 19:
    * Pages in list are sorted in FIFO order
    * R bits are cleared regularly
    * If the R bit of the oldest page is set it is put at the end of the list
    * If all the pages in the list have been referenced the page that was “recycled” will reappear with the R bit cleared and will be thrown away.

[^first]: https://sites.cs.ucsb.edu/~chris/teaching/cs170/doc/cs170-08.pdf
[^second]:https://en.wikipedia.org/wiki/Page_replacement_algorithm#Least_recently_used

2. A small computer has 8 page frames, each containing a page. The page frames contain virtual pages A, C, G, H, B, L, N , and D in that order. Their respective load times were 18, 23, 5, 7, 32, 19, 3, and 8. Their reference bits are 1, 0, 1, 1, 0, 1, 1, and 0 and their modified bits are 1, 1, 1, 0, 0, 0, 1, and 1, respectively. Which page will the second chance page replacement algorithm replace?
D.

Construct the FIFO queue first.
N -> G-> H -> D -> A -> L -> C -> B 
Then, some as question 1's (d) algorithm.
Therefore we can find D.

3. What is the difference between a physical address and a virtual address?

* Physical addresses refer to hardware addresses of physical memory.
* Virtual addresses refer to the virtual store viewed by the process.
* Virtual addresses might be the same as physical addresses; might be different, in which case virtual addresses must be mapped into physical addresses.
* Virtual space is limited by size of virtual addresses. (not physical addresses)
* Virtual space and physical memory space are independent.

4. Are there any circumstances in which clock and second chance choose different pages to replace? If so, what are they?
Well, it's same. Just in different presentation.


5. A small computer has four page frames. At the first clock tick, the R bits are 0111 (page 0 is 0, the rest are 1). At subsequent clock ticks, the values are 1011, 1010, 1101, 0010, 1010, 1100, and 0001. If the aging algorithm is used with an 8-bit counter, give the values of the four counters after the last ticks.

| pages  | counter  |
| ------ | -------- |
| page 0 | 01101110 |
| page 1 | 01001001 |
| page 2 | 00110111 |
| page 3 | 10001011 |
Actually, it is a "Counting-Based Page Replacement" algorithm.

* Counting-Based Page Replacement
    * To shift the counts right by 1 bit at regular intervals, forming an exponentially decaying average usage count.
