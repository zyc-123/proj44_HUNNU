#include "self_def.h"
#include "macro_def.h"
#include "k_obj_ref.c"

kstat_t krhino_rwlock_create(krwlock_t *rwlock, const name_t *name){

    NULL_PARA_CHK(rwlock);
    NULL_PARA_CHK(name);

    memset(rwlock, 0, sizeof(krwlock_t));

    /* Initialization */
    rwlock->rw_cond_readers.blk_list.next = NULL;
    rwlock->rw_cond_readers.blk_list.prev = NULL;
    rwlock->rw_cond_writers.blk_list.next = NULL;
    rwlock->rw_cond_writers.blk_list.prev = NULL;

    rwlock->owner_list.next = NULL;
    rwlock->owner_list.prev = NULL;
    // initialize the mutex
    kmutex_t* rw_mutex = rwlock->rw_mutex;
    krhino_mutex_create(rw_mutex,name);

    return RHINO_SUCCESS;
}

kstat_t krhino_rwlock_del(krwlock_t *rwlock){

}

kstat_t krhino_rwlock_rdlock(krwlock_t *rwlock){
    
    NULL_PARA_CHK(rwlock);

    // Occupy the mutex
    krhino_mutex_lock(rwlock->rw_mutex,RHINO_WAIT_FOREVER);

    // If relock, do something...

    // If rwlock can be get by reader
    if (rwlock->ref_count >= 0)
    {
        if (rwlock->ref_count == (unsigned int)(-1))
        {
            /* fatal error here, system must be stoped here */
            //k_err_proc(RHINO_MUTEX_NESTED_OVF);
            //RHINO_CRITICAL_EXIT();
            //return RHINO_MUTEX_NESTED_OVF;
        }
        
        (rwlock->ref_count)++;
        return RHINO_SUCCESS;
    }

    // Can't get the rwlock, block the current read task after unlock the mutex
    pend_to_read_blk_list(rwlock->rw_mutex);

    // Wake up, do something...
}

kstat_t krhino_rwlock_rwlock(krwlock_t *rwlock){

    NULL_PARA_CHK(rwlock);
    // Occupy the mutex
    krhino_mutex_lock(rwlock->rw_mutex,RHINO_WAIT_FOREVER);

    // If relock, do something...

    // If rwlock can be get by writer
    if (rwlock->ref_count == 0)
    {
        (rwlock->ref_count)--;
        return RHINO_SUCCESS;
    }

    // Can't get the rwlock, block the current write task after unlock the mutex
    pend_to_writer_blk_list(rwlock->rw_mutex);

    // Wake up, do something...
}

kstat_t krhino_rwlock_unlock(krwlock_t *rwlock){

    // Some check
    NULL_PARA_CHK(rwlock);

    uint8_t  cur_cpu_num;

    // Try to find the current task in the owner list
    klist_t* owner = rwlock->owner_list.next;
    k_rwlock_owner* rwlock_owner = NULL;
    while (owner)
    {
        rwlock_owner = (k_rwlock_owner*)((uint32_t)owner - (uint32_t)(&(((k_rwlock_owner*)0u)->blk_list)));
        if (g_active_task[cur_cpu_num] == rwlock_owner->rwlock_task)
        {
            break;
        }
        owner = owner->next;
    }

    krhino_mutex_lock(rwlock->rw_mutex,RHINO_WAIT_FOREVER);

    // If id one of the owner
    if (owner)
    {
        /* Reader unlock */
        if (rwlock->ref_count > 0)
        {
            (rwlock_owner->owner_nested)--;
            if ((rwlock_owner->owner_nested) <= 0)//task unlock all the nest
            {
                // refcount decrease by 1
                (rwlock->ref_count)--;
                // remove from the owner list
                owner->prev->next = owner->next;
                owner->next->prev = owner->prev;
            }
        }
        /* Writer unlock */
        else if (rwlock->ref_count < 0)
        {
            (rwlock_owner->owner_nested)--;
            if ((rwlock_owner->owner_nested) <= 0)//task unlock all the nest
            {
                // refcount reset to 0
                (rwlock->ref_count) = 0;
                // remove from the owner list
                owner->prev->next = owner->next;
                owner->next->prev = owner->prev;
            }
        }
        else
        {
            // unlock a free rwlock prohibited, do something...
        }
        
        
    }

    krhino_mutex_unlock(rwlock->rw_mutex);
    
}

kstat_t krhino_cond_signal(){

}

