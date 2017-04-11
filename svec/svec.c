#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "svec.h"

#define MIN_SIZE 64

int grow(sync_vec_t * vector) {
    void * tmp = realloc(vector->array, vector->allocated_slots * 2 * vector->element_size);
    if (tmp == NULL) 
        return VEC_COULD_NOT_ALLOCATE_MEMORY;

    vector->array = tmp;
    vector->allocated_slots *= 2;

    return VEC_SUCCESS;
}

int shrink(sync_vec_t * vector) {
    void * tmp = realloc(vector->array, vector->allocated_slots * vector->element_size / 2);
    if (tmp == NULL) 
        return VEC_COULD_NOT_ALLOCATE_MEMORY;

    vector->array = tmp;
    vector->allocated_slots /= 2;

    return VEC_SUCCESS;
}

int sync_init (sync_vec_t * vector, size_t element_size) {
    //check already initialized
    if (vector->array != NULL && vector->allocated_slots > 0) 
        return VEC_ALREADY_INITIALIZED;

    vector->element_size = element_size;
    vector->used_slots = 0;

    vector->allocated_slots = MIN_SIZE;
    vector->array = calloc(MIN_SIZE, element_size);

    //check memory allocation
    if (vector->array == NULL) 
        return VEC_COULD_NOT_ALLOCATE_MEMORY;

    sem_init(&(vector->lock), 0, 1);

    //everything is good
    return VEC_SUCCESS;
}
int sync_insert(sync_vec_t * vector, void * element_ptr, int idx) {
    int i;
    sem_wait(&(vector->lock));
    //check that have enough space, grow if necesary
    if (vector->used_slots == vector->allocated_slots - 1) {
        if (grow(vector)) {
            sem_post(&(vector->lock));
            return VEC_COULD_NOT_ALLOCATE_MEMORY;

        }
    }

    //check bounds
    if (!(idx >=0 && (size_t)idx <= vector->used_slots)) {
        sem_post(&(vector->lock));
        return VEC_INDEX_OUT_OF_BOUNDS;
    }
    
    //move everything over one
    for (i = vector->used_slots; i > idx; i--) {
        memcpy(vector->array + (i) * vector->element_size,
                vector->array + (i - 1) * vector->element_size,
                vector->element_size);
    }
    //insert
    memcpy(vector->array + idx * vector->element_size, element_ptr, vector->element_size);

    vector->used_slots++;

    sem_post(&(vector->lock));
    return VEC_SUCCESS;

}

int sync_replace(sync_vec_t * vector, void * element_ptr, int idx) {
    sem_wait(&(vector->lock));
    //check bounds
    if (!(idx >=0 && (size_t)idx < vector->used_slots)) {
        sem_post(&(vector->lock));
        return VEC_INDEX_OUT_OF_BOUNDS;
    }

    //overwrite the other thing
    memcpy(vector->array + idx * vector->element_size, element_ptr, vector->element_size);
    sem_post(&(vector->lock));
    return VEC_SUCCESS;

}

int sync_append(sync_vec_t * vector, void * element_ptr) {
    return sync_insert(vector, element_ptr, vector->used_slots);
}
int sync_veclen(sync_vec_t * vector) {
    return vector->used_slots;
}
int sync_remove_index(sync_vec_t * vector, int idx) {
    int res;
    sem_wait(&(vector->lock));
    res = remove_index(vector, idx);
    sem_post(&(vector->lock));
    return res;
}
int remove_index(sync_vec_t * vector, int idx) {
    int i;
    //check bounds
    if (!(idx >=0 && (size_t)idx < vector->used_slots)) 
        return VEC_INDEX_OUT_OF_BOUNDS;
    
    //move everything over one
    for (i = idx; (size_t)i < vector-> used_slots; i++) {
        memcpy(vector->array + (i) * vector->element_size,
                vector->array + (i + 1) * vector->element_size,
                vector->element_size);
    }

    //zero out what was last
    memset(vector->array + (--vector->used_slots) * vector->element_size, 0, vector->element_size);

    //shrink if now use 1/4 space as allocated
    if (vector->used_slots < vector->allocated_slots/4 && vector->allocated_slots > MIN_SIZE) {
        if (!shrink(vector))
            return VEC_COULD_NOT_ALLOCATE_MEMORY;
    }

    return VEC_SUCCESS;
}
int sync_remove_element(sync_vec_t * vector, void * element_ptr) {
    int i;
    sem_wait(&(vector->lock));
    if (element_ptr == NULL) {
        sem_post(&(vector->lock));
        return VEC_NULL_BUFFER;
    }

    for (i = 0; (size_t)i < vector->used_slots; i++) {
        if (memcmp(vector->array + i * vector->element_size, element_ptr, vector->element_size) == 0) {
            sem_post(&(vector->lock));
            return remove_index(vector, i);
        }
    }
    sem_post(&(vector->lock));
    return VEC_NOT_FOUND;
}
int sync_get(sync_vec_t * vector, int idx, void * element_buffer) {
    sem_wait(&(vector->lock));
    if (element_buffer == NULL) {
        sem_post(&(vector->lock));
        return VEC_NULL_BUFFER;
    }

    //check bounds
    if (!(idx >=0 && (size_t)idx < vector->used_slots)) {
        sem_post(&(vector->lock));
        return VEC_INDEX_OUT_OF_BOUNDS;
    }

    memcpy(element_buffer, vector->array + idx * vector->element_size, vector->element_size);
    sem_post(&(vector->lock));

    return VEC_SUCCESS;
}
int sync_destroy(sync_vec_t * vector) {
    int sem_val;
    if (vector->array == NULL)
        return VEC_ALREADY_DESTROYED;

    free(vector->array);
    memset(vector, 0,sizeof(sync_vec_t));


    do {
        sem_getvalue(&(vector->lock), &sem_val);
    } while (sem_val > 0);
    sem_destroy(&(vector->lock));

    return VEC_SUCCESS;
}
int sync_sort(sync_vec_t * vector, cmpfn cmp) {
    sem_wait(&(vector->lock));
    qsort(vector->array,vector->used_slots, vector->element_size,cmp);
    sem_post(&(vector->lock));
    return VEC_SUCCESS;
}

int sync_copy(sync_vec_t * srcvec, sync_vec_t * dstvec) {
    sem_wait(&(srcvec->lock));
    //check already initialized
    if (dstvec->array != NULL && dstvec->allocated_slots > 0) {
        sem_post(&(srcvec->lock));
        return VEC_ALREADY_INITIALIZED;
    }

    dstvec->element_size = srcvec->element_size;
    dstvec->used_slots = srcvec->used_slots;
    dstvec->allocated_slots = srcvec->allocated_slots;
    dstvec->array = calloc(srcvec->allocated_slots, srcvec->element_size);

    //check memory allocation
    if (dstvec->array == NULL) {
        sem_post(&(srcvec->lock));
        return VEC_COULD_NOT_ALLOCATE_MEMORY;
    }
    
    memcpy(dstvec->array, srcvec->array, srcvec->allocated_slots * srcvec->element_size);

    //everything is good
    sem_post(&(srcvec->lock));
    return VEC_SUCCESS;
}

int sync_to_array(sync_vec_t * vec, void ** resultptr) {
    sem_wait(&(vec->lock));
    void * result = calloc(vec->used_slots, vec->element_size);
    if (!result) {
        sem_post(&(vec->lock));
        return VEC_COULD_NOT_ALLOCATE_MEMORY;
    }

    memcpy(result, vec->array, vec->used_slots * vec->element_size);

    *resultptr = result;

    sem_post(&(vec->lock));
    return VEC_SUCCESS;
}
