part2 名義上是說是要做一個記憶體管理器
但是事實上，我們其實要做的是 malloc 系列的功能

這邊有一個重要的 syscall，就是`brk`
`brk`主要的工作是幫你把虛擬記憶體對應到實體記憶體，至於這兩個 syscall 底層如何實作，要去看 glibc 內部的實作。
在 Intel 的 x86 架構中是這樣寫的：一層包一層還是沒有很明瞭... [source code](https://code.woboq.org/userspace/glibc/sysdeps/unix/sysv/linux/x86_64/brk.c.html)

話說，這邊可能是我第一次講到 syscall 這個詞(在我目前寫這得時候應該是)，syscall 念作 system call，我覺得 wiki 寫的算是淺顯易懂，我就放 wiki 了
[Wikipedia](https://zh.wikipedia.org/zh-tw/%E7%B3%BB%E7%BB%9F%E8%B0%83%E7%94%A8)

好，那，`sbrk` 到底是幹嘛？
前面說 `brk` 是幫你把虛擬記憶體對應上去嘛... 然後他還會幫你把 data 段的那條線往上移，最後 return 的是【上移之後的虛擬記憶體位置】。
那 `sbrk` 就是幫你包裝一層 `brk`，他會幫你記得 data 段是到哪裡，你只要給他一個大小，他就會幫你推上去。
(這邊嚴格講應該說 bss 段，可是 bss 段也是在廣義的 data 段裡面)

其他參考資料可以看看 [ITREAD](https://www.itread01.com/p/1390558.html)
(我沒看完，裡面可能也有勘誤，如有勘誤歡迎提供 PR)

---

好那，我們要怎麼寫 `malloc`?
觀念就是，你至少要建一個 Linked list 去紀錄，你被使用者 free 掉的記憶體，如果下次使用者在要(ask for)記憶體的時候，先從已經 free 掉的裡面找合適的，如果沒有合適的再去 `sbrk`。
這邊有個專有名詞叫做 [free list](https://zh.wikipedia.org/zh-tw/%E8%87%AA%E7%94%B1%E8%A1%A8) 就是在指這個 linked list。

那為啥不要每次都 `sbrk`？
> 因為 `sbrk` 是有一定成本的，他要先 call 到 glibc 再去 call 一個 syscall `brk` 到 kernel space，而進出 kernel space 都有一定時間成本(因為 kernel 不能直接接露太多資訊給 user，這是因為 kernel 就是可以掌管全部 service 的資源了， kernel 如果亂給你，其他程式豈不是看光光？)。
> 所以他可能會用 `copy_to_user` (我不知道)之類的 function 傳回來給 user space。然後還要考慮 TLB, page in, page out, mmap 中間還有很多東西要做...還要考慮去硬體(實體記憶體)找可用空間的時間...因此有不小的成本。
> 所以頻繁進出 kernel space 是會造成效能上的瓶頸，所以設計出一個 pool 先把資源裝起來，下次要資源就不用到 kernel space 去浪費時間。

下面是作業的延伸閱讀...(自己也沒看完，寫得很心虛)

這邊還可以優化的地方是：
> free list 用 rbtree 做，這樣可以保證在 $O(logN)$ 的時間找 free list
> 記憶體對齊的問題
> 一次多要點空間下次切著用...
> 對不同大小的空間要求做不同的應對實作：就像 glibc 在：
> * 大request(>= 512 bytes) : 使用best-fit的策略，在一個 range(bin)的 list 中尋找
> * 小request(<= 64 bytes) : caching allocation，保留一系列固定大小區塊的 list 以利迅速回收再使用
> * 在兩者之間 : 試著結合兩種方法，有每個 size 的 list (小的特性)，也有合併(coalesce)機制、double linked list(大)
> * 極大的 request(>= 128KB by default) : 直接使用 mmap()，讓 memory management 解決

這些我也沒做，歡迎給我 PR

---

最後就是看看人家 glibc 的 malloc 怎麼做?
[source code](https://code.woboq.org/userspace/glibc/malloc/malloc.c.html)

名詞解釋：
發現 jserv 有寫過，我就不寫了
[glibc 的 malloc/free 實作](https://hackmd.io/@sysprog/c-memory?type=view#glibc-%E7%9A%84-mallocfree-%E5%AF%A6%E4%BD%9C)

祝你寫扣順利