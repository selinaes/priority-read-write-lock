# Sync Lab

In this lab your will implement a priority read/write lock library with mutex. 

## Lab setups

We use a different repo for the DMM lab. Please clone the repo via one of these addresses (depending on if you want HTTPS or SSH authentication):


        $ git clone https://gitlab.oit.duke.edu/os-course/sync-new-fall22.git
        OR
        $ git clone git@gitlab.oit.duke.edu:os-course/sync-new-fall22.git
        Cloning into 'sync-new-fall22'...

​
You need to run the lab on Linux system. We recommend you use the docker we provide (the same xv6 docker) as following commands:
​

        $ cd <PathToBaseRepo>
        $ docker run -it --rm -v $PWD:/home/xv6/xv6-riscv iqicheng/cps310-env
​

You only need to modify `rwlock.c` and `rwlock.h` to finish this lab.
​
To test your solution, please run <kbd>make test</kbd>. Each test case (TC) will print out "Test Passed!" or "Test Failed!". You need to pass all the test cases.
​
To submit your solution, please run <kbd>make gradescope</kbd>, and submit your `submission.zip` to gradescope.

## Intro of rwlock logistics
It is helpful to understand read/write lock from course slides or chapter 31.5 in [OSTEP read/write lock](https://pages.cs.wisc.edu/~remzi/OSTEP/threads-sema.pdf). Here we give the basic logistics of the priority rwlock in this lab.

1. Any number of threads can acquire the lock as a reader without blocking one another. 
2. Only one thread can acquire the lock as a writer.
3. The rwlock is writer-prefered, which means when there is a waiting writer, it will always acquire the lock before any waiting reader.
4. A waiting writer thread will block until all readers release the lock or the active writer release the lock. Any waiting reader will block until the active writer release the lock and there is no waiting writer thread.
5. The rwlock supports three priority levels: high, medium and low. For simplicity, we represent these levels with 0, 1 and 2. The writer with higher priority level will always beat the lower priority writer in acquiring the lock, i.e. writer with priority 0 will always lock before writer with priority 1, writer with priority 1 will always lock before writer with priority 2. However, writers in the same priority level compete equally.

## Implement rwlock

We provide the structure of read/write lock and basic functions in 'rwlock.h'.

        typedef struct {
            pthread_mutex_t     mutex;
            pthread_cond_t      r_cond;
            pthread_cond_t      w_cond[3];
            int                 r_active;
            int                 w_active[3];
            int                 r_wait;
            int                 w_wait[3];
        }rwl;

You will implement five methods that handles the read/write lock behavior.

        void rwl_init(rwl *l);
        void rwl_rlock(rwl *l);
        void rwl_runlock(rwl *l);
        void rwl_wlock(rwl *l, int priority);
        void rwl_wunlock(rwl *l, int priority);

## Hints
1. Use different condition variables for different priority levels.
2. Always keep track of active/waiting readers and writers.
2. Feel free to add helper functions or modify the lock struct if you need to, but don't change the five methods.

## Tests

You can test and grade your rwlock library locally by running

  make test

