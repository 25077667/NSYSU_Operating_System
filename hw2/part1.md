---
title: Homework 2 Part1
author: B073040047
date: 4/18/2020
---
###### tags:OS2020Spring

1. Yes
    Consider we have an instruction is "swap", can swap the contents of a register and a memory word in a single indivisible action.
    
    We can:
    ```assembly=
    ENTER_REGION:
        mov REGISTER, 1
        swap REGISTER, LOCK
        cmp LOCK, 0
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
    cmp LOCK, 0b
    jne ENTER_REGION
    ret

LEAVE_REGION:
    mov LOCK, 0b
    ret
```

2. 
Let f(Q) be the function of CPU efficiency.

* (a) $\lim_{Q\to\infty} f(Q)$
    $Q \ge T, \forall T \in \mathbb{R}$
    Hence there is no context switching.
    Therefore the $f(Q) = {T \over T} = 100\%$

* (b) $Q \gt T$
    Well, $Q \gt T$ is $\subset Q \ge T$
    Hence there is also no context switching.
    Therefore the $f(Q) = {T \over T} = 100\%$ too

* \(c\) $S \lt Q \lt T$
    It will context switch, Hence the cost time is determined on the times of context switching.
    The times of context switching is $m = \lfloor {T \over Q} \rfloor$
    Therefore the $f(Q) = {T \over {T + S * m}}$

* (d) $Q = S$
    By previous conclusion, and let $Q = S$.
    $f(Q) = {T \over 2T} = {1 \over 2}$

* (e) $\lim_{Q\to0} f(Q)$
    By \(c\)'s conclusion, $\lim_{Q\to0} f(Q) = 0$

3. Any order of create.

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
But the different is:
A and B raced by fork.
C and D raced by thread.
But before print "B = 1", must went to fork. That's cost time.
So, A has high probability goes before printing B.

Consider the race in B, C and D.
C and D are created by "pthread", which is also an expensive function call.
Hence, B has high probability goes before C and D.

Good, then consider C and D.
C and D are in the same process, but just competing in threads.
We expect the probability of D goes before C is 50%.

But, If you are unfortunate, the schedular does not pick your process/thread up.
Any order is possible.
