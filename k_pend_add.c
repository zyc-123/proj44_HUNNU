#include "self_def.h"

void pend_to_reader_blk_list(kmutex_t* rw_mutex){


    
    // Release the mutex
    krhino_mutex_unlock(rw_mutex);
}

void pend_to_writer_blk_list(kmutex_t* rw_mutex){



    // Release the mutex
    krhino_mutex_unlock(rw_mutex);
}