#ifndef SELF_DEF
#define SELF_DEF

#include "extern_def.h"

/*
 *
 * 第一题：实现不同的读写锁操作模式
 *  读操作优先：允许最大读并发。即当已经有读者或写者加锁时，后续到来的读请求优先于写请求得到满足。
 *  写操作优先：解决“读操作优先”模式下写请求容易饿死的问题。即当已经有读者或写者加锁时，后续到来的写请求优先于读请求得到满足。
 *
 * 第二题：实现读写锁的可重入机制
 *  允许同一个线程对同一个读写锁进行多次加锁与解锁操作。
 *  比如同一个线程可以对同一个读写锁加锁N次，当完成N次释放锁操作之后，该线程才这正释放锁。
 *
 */

/* definition of the wrlock owner */
typedef struct k_rwlock_owner
{
    int owner_nested;           /* Nested */
    klist_t       blk_list;     /* Manage owners, unidirectional */
    ktask_t*      rwlock_task;  /* TSP pointer */
} k_rwlock_owner;

/* definition of condition variable */
typedef struct k_rwlock_cond_t
{
    klist_t       blk_list;     /* Manage blocked tasks */
} k_rwlock_cond_t;



/* definition of read/write lock */
typedef struct rwlock_s{
    /* used for locking/unlocking a krwlock_t */
    kmutex_t* rw_mutex;

    /* condtion variables: read & write */
    k_rwlock_cond_t rw_cond_readers;
    k_rwlock_cond_t rw_cond_writers;
    /*
    -----------------------------------------------------
    |   ref_count   |   Status                          |
    -----------------------------------------------------
    |   < 0         |   total nested for read           |
    -----------------------------------------------------
    |   = 0         |   free                            |
    -----------------------------------------------------
    |   -1          |   write                           |
    -----------------------------------------------------
    */
    int ref_count;

    /* owner list, unidirectional */
    klist_t owner_list;

} krwlock_t;

/**
 * Create a read/write lock
 *
 * @param[in]   rwlock  read/write lock to create
 * @param[in]   name    name of the rwlock
 * 
 * @return   the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_rwlock_create(krwlock_t *rwlock, const name_t *name);

/**
 * Delete a read/write lock
 *
 * @param[in]   rwlock  read/write lock to delete
 * 
 * @return   the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_rwlock_del(krwlock_t *rwlock);

/**
 * Lock the read/write lock by reader, maybe be blocked
 * 
 * @param[in]   rwlock  read/write to lock by reader
 * 
 * @return   the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_rwlock_rdlock(krwlock_t *rwlock);

/**
 * Lock the read/write lock by writer, maybe be blocked
 * 
 * @param[in]   rwlock  read/write to lock by writer
 * 
 * @return   the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_rwlock_wrlock(krwlock_t *rwlock);

/**
 * Unlock the reader/writer
 * 
 * @param[in]   rwlock  read/write to unlock
 * 
 * @return   the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_rwlock_unlock(krwlock_t *rwlock);

/**
 * Arouse the blocked task on the currrent rwlock 
 * 
 * @param[in]
 * 
 * @return   the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_cond_signal();

/**
 * Block the current reader task
 * 
 * @param[in]
 * 
 * @return   void
 */
void pend_to_reader_blk_list(kmutex_t* mutex);

/**
 * Blocked the current writer task
 * 
 * @param[in]
 * 
 * @return   void
 */
void pend_to_writer_blk_list(kmutex_t* mutex);

#endif