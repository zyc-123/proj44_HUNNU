#ifndef EXTERN_DEF
#define EXTERN_DEF

#define INT_MAX       2147483647    // maximum (signed) int value
#define UINT32_MAX       0xffffffffui32
typedef unsigned char uint8_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;
typedef uint64_t tick_t;
/**
 * mutex object
 */
typedef struct mutex_s {
int c;
} kmutex_t;

typedef char     name_t;

/* Define basic errno for rhino modules */
typedef enum
{
    RHINO_SUCCESS = 0u,
    RHINO_SYS_FATAL_ERR,
    RHINO_SYS_SP_ERR,
    RHINO_RUNNING,
    RHINO_STOPPED,
    RHINO_INV_PARAM,
    RHINO_NULL_PTR,
    RHINO_INV_ALIGN,
    RHINO_KOBJ_TYPE_ERR,
    RHINO_KOBJ_DEL_ERR,
    RHINO_KOBJ_DOCKER_EXIST,
    RHINO_KOBJ_BLK,
    RHINO_KOBJ_SET_FULL,
    RHINO_NOTIFY_FUNC_EXIST,

    RHINO_MM_POOL_SIZE_ERR = 100u,
    RHINO_MM_ALLOC_SIZE_ERR,
    RHINO_MM_FREE_ADDR_ERR,
    RHINO_MM_CORRUPT_ERR,
    RHINO_DYN_MEM_PROC_ERR,
    RHINO_NO_MEM,
    RHINO_RINGBUF_FULL,
    RHINO_RINGBUF_EMPTY,

    RHINO_SCHED_DISABLE = 200u,
    RHINO_SCHED_ALREADY_ENABLED,
    RHINO_SCHED_LOCK_COUNT_OVF,
    RHINO_INV_SCHED_WAY,

    RHINO_TASK_INV_STACK_SIZE = 300u,
    RHINO_TASK_NOT_SUSPENDED,
    RHINO_TASK_DEL_NOT_ALLOWED,
    RHINO_TASK_SUSPEND_NOT_ALLOWED,
    RHINO_TASK_CANCELED,
    RHINO_SUSPENDED_COUNT_OVF,
    RHINO_BEYOND_MAX_PRI,
    RHINO_PRI_CHG_NOT_ALLOWED,
    RHINO_INV_TASK_STATE,
    RHINO_IDLE_TASK_EXIST,

    RHINO_NO_PEND_WAIT = 400u,
    RHINO_BLK_ABORT,
    RHINO_BLK_TIMEOUT,
    RHINO_BLK_DEL,
    RHINO_BLK_INV_STATE,
    RHINO_BLK_POOL_SIZE_ERR,

    RHINO_TIMER_STATE_INV = 500u,

    RHINO_NO_THIS_EVENT_OPT = 600u,

    RHINO_BUF_QUEUE_INV_SIZE = 700u,
    RHINO_BUF_QUEUE_SIZE_ZERO,
    RHINO_BUF_QUEUE_FULL,
    RHINO_BUF_QUEUE_MSG_SIZE_OVERFLOW,
    RHINO_QUEUE_FULL,
    RHINO_QUEUE_NOT_FULL,

    RHINO_SEM_OVF = 800u,
    RHINO_SEM_TASK_WAITING,

    RHINO_MUTEX_NOT_RELEASED_BY_OWNER = 900u,
    RHINO_MUTEX_OWNER_NESTED,
    RHINO_MUTEX_NESTED_OVF,

    RHINO_NOT_CALLED_BY_INTRPT = 1000u,
    RHINO_TRY_AGAIN,

    RHINO_WORKQUEUE_EXIST = 1100u,
    RHINO_WORKQUEUE_NOT_EXIST,
    RHINO_WORKQUEUE_WORK_EXIST,
    RHINO_WORKQUEUE_BUSY,
    RHINO_WORKQUEUE_WORK_RUNNING,

    RHINO_TASK_STACK_OVF = 1200u,
    RHINO_INTRPT_STACK_OVF,

    RHINO_STATE_ALIGN = INT_MAX /* keep enum 4 bytes at 32bit machine */
} kstat_t;


/**
 * As a member of other structures, 'klist_t' can form a doubly linked list.
 */
typedef struct klist_s {
    struct klist_s *next;
    struct klist_s *prev;
} klist_t;

/**
 * Blocking strategy.
 * It determines which task can go first when they are waiting for one resource.(from 'pend' to 'ready')
 * BLK_POLICY_PRI is default.
 */
typedef enum {
    /**
     * High-priority tasks are easier to obtain resources, the same priority is in FIFO order.
     */
    BLK_POLICY_PRI = 0u,
    /**
     * Just in FIFO order.
     */
    BLK_POLICY_FIFO
} blk_policy_t;

/**
 * The objects types
 */
typedef enum {
    RHINO_OBJ_TYPE_NONE = 0,
    RHINO_SEM_OBJ_TYPE,
    RHINO_MUTEX_OBJ_TYPE,
    RHINO_QUEUE_OBJ_TYPE,
    RHINO_BUF_QUEUE_OBJ_TYPE,
    RHINO_TIMER_OBJ_TYPE,
    RHINO_EVENT_OBJ_TYPE,
    RHINO_MM_OBJ_TYPE
} kobj_type_t;

/**
 * Abstract model of the 'obj':
 * 'obj' is an abstraction of all types of objects that can cause task blocking
 * (that is, tasks waiting for the resource to enter the PEND state).
*/
typedef struct blk_obj {
    klist_t       blk_list;     /**< Manage blocked tasks */
    const name_t *name;
    blk_policy_t  blk_policy;
    kobj_type_t   obj_type;
#if (RHINO_CONFIG_USER_SPACE > 0)
    klist_t       obj_list;
#endif
#if (RHINO_CONFIG_TASK_DEL > 0)
    uint8_t       cancel;
#endif
} blk_obj_t;

/**
 * Create a mutex.
 *
 * @param[in] mutex     pointer to the mutex (the space is provided outside, by user)
 * @param[in] name      name of the mutex
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_create(kmutex_t *mutex, const name_t *name);

/**
 * Delete a mutex.
 *
 * @param[in] mutex     pointer to the mutex
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_del(kmutex_t *mutex);

#if (RHINO_CONFIG_KOBJ_DYN_ALLOC > 0)
/**
 * Malloc and create a mutex.
 *
 * @param[out] mutex    pointer to the mutex (the space is provided inside, from heap)
 * @param[in]  name     name of the mutex
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_dyn_create(kmutex_t **mutex, const name_t *name);

/**
 * Delete and free a mutex.
 *
 * @param[in] mutex     pointer to the mutex
 *
 * @return the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_dyn_del(kmutex_t *mutex);
#endif

/**
 * Lock mutex, task may be blocked.
 *
 * @param[in]  mutex    pointer to the mutex
 * @param[in]  ticks    ticks to be wait for before lock
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_lock(kmutex_t *mutex, tick_t ticks);

/**
 * Unlock a mutex.
 *
 * @param[in]  mutex    pointer to the mutex
 *
 * @return  the operation status, RHINO_SUCCESS is OK, others is error
 */
kstat_t krhino_mutex_unlock(kmutex_t *mutex);


#endif