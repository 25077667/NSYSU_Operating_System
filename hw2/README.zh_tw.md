part 2 是哲學家用餐問題

老實說，就是一個很古老的經典題目，這個問題把 deadlock 的情況寫的很簡單易懂
但是就是題目一點用處也沒有就是了(有點像高中古板的那種題型，簡單易懂，但一點無法重用 reuse)

所以故事是這樣的：
設有五位哲學家圍坐在一張圓形餐桌旁，做以下兩件事情之一：吃飯，或者思考。吃東西的時候，他們就停止思考，思考的時候也停止吃東西。餐桌中間有一大碗義大利麵，每位哲學家之間各有一支餐叉。因為用一支餐叉很難吃到義大利麵，所以假設哲學家必須用兩支餐叉吃東西。他們只能使用自己左右手邊的那兩支餐叉。哲學家就餐問題有時也用米飯和五根筷子而不是義大利麵和餐叉來描述，因為吃米飯必須用兩根筷子。
哲學家從來不交談，這就很危險，可能產生死結，每個哲學家都拿著左手的餐叉，永遠都在等右邊的餐叉（或者相反）。

解法的話， [Wiki](https://zh.wikipedia.org/zh-tw/%E5%93%B2%E5%AD%A6%E5%AE%B6%E5%B0%B1%E9%A4%90%E9%97%AE%E9%A2%98#%E8%A7%A3%E6%B3%95) 有提供三種解法
我這邊的作法是第三種：Chandy/Misra解法

只是我是去改狀態(Semaphore 會叫起等待資源的哲學家)，沒有拿餐券。
總之，就是有人下去吃東西，就會把筷子的那個 mutex(互斥鎖) lock 起來，然後把競爭變數壓下去(form C++ condition variable pthread API)
如果這時候有哲學家也請求這支筷子，後來請求的哲學家會去睡覺(Condition variable)，等待正在使用的哲學家用完餐，把睡覺的哲學家叫起床。
<這段，詳見 semaphore.cpp 跟 semaphore.hpp>

對，我就這樣很輕鬆地透過 C++ 提供的 condition_variable 跟 mutex 把筷子這個會造成資源競爭的東西，丟給 standard 去實作。
我不用去 care 到底要叫誰，會不會 deadlock，因為 C++ 的 condition_variable 可以幫我做到 busy waiting

這是 condition variable wait 在 include/condition_variable 內的實作
![Imgur](https://imgur.com/2XvFf2N.png)
可以看出用 while 去 busy waiting

---

其他比較需要說的會是 `random_func`
(因為剩下的我註解應該都有說)

這是一個很優雅的用法(From C++11)，這稱作高階函式(high-order function)，就是你常聽到得 functional programming 的必備要素
High-order function 的定義是 input 至少為一個函式(函數)作為參數，而輸出式一個函式(函數)

至於這邊是一個簡單的應用，把亂數裝置(random device)展生亂數種子的函式跟一個均值分布的 class 綁再一起，產生出一個新的函式，在透過這個新函式的取值，產生出一個均值分布的亂數。

其他詳細，可以查閱 C++ functional programming

或是對這個亂數生成有興趣的話，可以找 C++11 Random library，(都幾年了，還在用 srand 丟時間很雷)

最後就是對數學有興趣的話，可以找找梅森質數旋轉法(當然仍舊是偽隨機，真隨機就去 /dev/random 裡面拿吧，只是那個累積 entropy 的時間成本自負)