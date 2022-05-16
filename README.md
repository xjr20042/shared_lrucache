# shared_lrucache
multiprocess shared memory LRU key-value cache

一个跨进程的共享的LRU key-value 缓存, 内部使用文件锁确保进程间访问安全。

A shared LRU key-value cache across processes, internally using file locks to secure interprocess access.  
