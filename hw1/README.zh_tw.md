part 1 如同 part1.md

part 2 這邊將會有幾個版本：

1. hacking(execve)
2. file system(powerful)
3. fork(lite)
4. fork(powerful)

**要把這邊的資料夾作編號**

## hacking
其實這邊是一個 trick，我直接叫一個真正的 shell 起來，我根本就不用寫 shell

但是，這樣不好，不是老師要我們做的東西。

## file system
其實如果要做 pipe，標準上無法保證 process 的執行先後順序。

但是我如果透過 popen() 這 API 就可以讓一個 process 的 stdout 到一個 file pointer

這樣就可以嚴格保證他 ripple 前進。

## fork(lite)
只完成老師要求，並且盡可能縮短程式碼(會有註解)
嚴格保證使用者可以輸入任意長度指令(採動態配置記憶體)
> 目前 163 行(2020/07/13)
```
-------------------------------------------------------------------------------
Language                     files          blank        comment           code
-------------------------------------------------------------------------------
C                                2             17             64            163
C/C++ Header                     1              5             15             18
make                             1              5              0             16
-------------------------------------------------------------------------------
SUM:                             4             27             79            197
-------------------------------------------------------------------------------
```

## fork(powerful)
目前是希望能做出以下功能：

1. 滿足作業要求
2. 支援方向鍵編輯指令
3. 可以使用 wildcard
4. 可以使用 Tab 鍵來 auto compelete
    * 這需要安裝 [GNU Readline Library](https://tiswww.case.edu/php/chet/readline/rltop.html)
    * 我會另外寫 Readme 在那資料夾裡面
    * Double Tab 應該會放進來
5. 支援 alias, variable(包含其他 shell 常見預設變數), loop
6. 自由設定 prompt
    * 就像 zsh 美美的 theme
7. 可以支援 shell script
    * 參考 [bash](https://www.gnu.org/software/bash/manual/bash.html)
8. 其他盡可能參考 [bash](https://www.gnu.org/software/bash/manual/bash.html) 特色來實作