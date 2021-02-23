這份作業沒麼好說明的，主要從 man 裡面抄下來，依樣畫葫蘆操作 mmap 並不困難。

（所以你會看到我很多用詞，macro 都是跟 man 相同，其實就是為了保留原本）


這邊需要注意的是 write.c 裡面有一個 dummy byte 要寫。

老實說，我還不知道為什麼需要寫這一個 dummy byte，但是不寫會無法 mmap

如果你知道為什麼，歡迎發 issue 告訴我，我會整合說明進去。


另外還需要小心一點的是 open 的 flags, mode。詳 [man-page](https://man7.org/linux/man-pages/man2/open.2.html)

我會這樣說，是因為有不少同學因為 flag 寫錯，demo 時權限不對（permission denied）。

不過還是建議去看一下 [man-page](https://man7.org/linux/man-pages/man2/open.2.html) 這樣對你比較有幫助