---
title: Homework 2 Part1
author: B073040047
date: 4/18/2020
---
###### tags:OS2020Spring

This homework in [Hackmd Link](https://hackmd.io/@25077667/os-hw2)
---

## 1. Consider a computer that does not have a TEST AND SET LOCK instruction but does have aninstruction to swap the contents of a register and a memory word in a single indivisible action. Can that beused to write a routineenterregionsuch as the one found in Fig. 2–12.
Yes.
    Consider we have an instruction is "swap", can swap the contents of a register and a memory word in a single indivisible action.
    
:::warning
Suppose the first process do clear the bit of `LOCK` after boot.
Or, there is no answer.
:::
We can:
```assembly=
ENTER_REGION:
    mov REGISTER, 1
    swap REGISTER, LOCK
    cmp REGISTER, 0
    jne ENTER_REGION
    ret

LEAVE_REGION:
    mov LOCK, 0
    ret
```
Actually, in x86, this `swap` instruction is named `xchg`.
Then it will looks like:
```assembly=
ENTER_REGION:
    mov REGISTER, 1b
    xchg REGISTER, LOCK
    cmp REGISTER, 0b
    jne ENTER_REGION
    ret

LEAVE_REGION:
    mov LOCK, 0b
    ret
```

## 2. Measurements of a certain system have shown that the average process runs for a time $T$ beforeb locking on I/O. A process switch requires a time $S$, which is effectively wasted (overhead).  For round-robin scheduling with quantum $Q$, give a formula for the CPU efficiency (i.e., the useful CPU time dividedby the total CPU time) for each of the following:
Let $f(Q)$ be the function of measuring CPU efficiency.

:::danger
$T$ is the **AVERAGE** time.
So, we need to consider the time of context swirtch $S$ in each case except for (a).
:::

* (a) $\lim_{Q \to \infty} f(Q)$
    $Q \ge T, \forall T \in \mathbb{R}$
    Hence there is no context switching.
    Therefore the $f(Q) = {T \over T} = 100\%$

* (b) $Q \gt T$
    There is a context switching.(Look the "danger zone")
    Therefore the $f(Q) = {T \over T + S}$

* \(c\) $S \lt Q \lt T$
    It will context switch. Hence the executing time is determined on the times of context switching.
    The times of context switching is $m = \lceil {T \over Q} \rceil$
    Therefore the $f(Q) = {T \over {T + S * m}}$

* (d) $Q = S, \ Q \in \mathbb{R}$
    * Consider $S = Q \lt T$:
    By previous conclusion, and let $Q = S$.
    $f(Q) = {T \over 2T} = {1 \over 2}$
    * Consider $S = Q \gt T$:
    The result is same as (a).

* (e) $\lim_{Q \to 0} f(Q)$
    By \(c\)'s conclusion, $\lim_{Q\to 0} f(Q) = 0$

## 3. Consider the interprocess-communication scheme where mailboxes are used.  Suppose a processPwants to wait for two messages, one from mailboxAand one from mailboxB. What sequence ofsendandreceiveshould it execute so that the messages can be received in any order?
Any order of `pthread_create()`. It is nothing to do with `send()`.

For example:
```cpp=
pthread_t *A = (pthread_t *)malloc(sizeof(pthread_t));
pthread_t *B = (pthread_t *)malloc(sizeof(pthread_t));
pthread_create(A, NULL, receive, msgA);
pthread_create(B, NULL, receive, msgB);
pthread_join(*A, NULL);
pthread_join(*B, NULL);
```

4. 
```
A = 1
B = 1
C = 2
D = 2
```
Any order for this four lines. Because they will compete.

But before print "B = 1", must went to `fork` again(Line 18). Be careful the `fork()` function is expensive. Printing "B = 1"(Line 20) will be executed behind the `fork`(Line 18).
So, A has a high probability goes before printing B.

Consider the race in B, C and D.
C and D are created by `fork` too.
But after forking this two processes, they called "pthread", which is also an expensive function call.
Hence, B has a high probability goes before C and D.

Good, then consider C and D.
C and D are in the different process. Both C and D have their own thread.(Same function definition but in different process)
The thread is created to increase the global variable called `value` then `join` the thread.
After the thread join, print C and D.
Therefore, C and D will compete by scheduler.

Ok, A, B, C and D are competing by `fork`. But the costing time by function call is:
A: fork
B: fork + fork
C: fork + fork + fork + thread
D: fork + fork + fork + thread
Hence, the expectation of these four process costing time before printing is:
$E(A) < E(B) < E(C) = E(D)$

But, If you are unfortunate, the schedular do not pick your process/thread up.
Any order is possible.
