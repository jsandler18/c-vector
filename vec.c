#include <stdlib.h>
#include <string.h>
#include "vec.h"

#define MIN_SIZE 64

int grow(vec_t * vector) {
    void * tmp = realloc(vector->array, vector->allocated_slots * 2 * vector->element_size);
    if (tmp == NULL) 
        return VEC_COULD_NOT_ALLOCATE_MEMORY;

    vector->array = tmp;
    vector->allocated_slots *= 2;

    return VEC_SUCCESS;
}

int shrink(vec_t * vector) {
    void * tmp = realloc(vector->array, vector->allocated_slots * vector->element_size / 2);
    if (tmp == NULL) 
        return VEC_COULD_NOT_ALLOCATE_MEMORY;

    vector->array = tmp;
    vector->allocated_slots /= 2;

    return VEC_SUCCESS;
}

int init (vec_t * vector, size_t element_size) {
    vector->element_size = element_size;
    vector->used_slots = 0;
    //check already initialized
    if (vector->array != NULL && vector->allocated_slots > 0) 
        return VEC_ALREADY_INITIALIZED;

    vector->allocated_slots = MIN_SIZE;
    vector->array = calloc(MIN_SIZE, element_size);

    //check memory allocation
    if (vector->array == NULL) 
        return VEC_COULD_NOT_ALLOCATE_MEMORY;

    //everything is good
    return VEC_SUCCESS;
}
int insert(vec_t * vector, void * element_ptr, int idx) {
    //check that have enough space, grow if necesary
    if (vector->used_slots == vector->allocated_slots - 1) {
        if (grow(vector)) 
            return VEC_COULD_NOT_ALLOCATE_MEMORY;
    }

    //check bounds
    if (!(idx >=0 && idx <= vector->used_slots)) 
        return VEC_INDEX_OUT_OF_BOUNDS;
    
    //move everything over one
    for (int i = vector->used_slots; i > idx; i--) {
        memcpy(vector->array + (i) * vector->element_size,
                vector->array + (i - 1) * vector->element_size,
                vector->element_size);
    }
    //insert
    memcpy(vector->array + idx * vector->element_size, element_ptr, vector->element_size);

    vector->used_slots++;

    return VEC_SUCCESS;

}
int append(vec_t * vector, void * element_ptr) {
    return insert(vector, element_ptr, vector->used_slots);
}
int veclen(vec_t * vector) {
    return vector->used_slots;
}
int remove_index(vec_t * vector, int idx) {
    //check bounds
    if (!(idx >=0 && idx < vector->used_slots)) 
        return VEC_INDEX_OUT_OF_BOUNDS;
    
    //move everything over one
    for (int i = idx; i < vector-> used_slots; i++) {
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
int remove_element(vec_t * vector, void * element_ptr) {
    if (element_ptr == NULL) 
        return VEC_NULL_BUFFER;

    for (int i = 0; i < vector->used_slots; i++) {
        if (memcmp(vector->array + i * vector->element_size, element_ptr, vector->element_size) == 0) {
            return remove_index(vector, i);
        }
    }
    return VEC_NOT_FOUND;
}
int get(vec_t * vector, int idx, void * element_buffer) {
    if (element_buffer == NULL) 
        return VEC_NULL_BUFFER;

    //check bounds
    if (!(idx >=0 && idx < vector->used_slots)) 
        return VEC_INDEX_OUT_OF_BOUNDS;

    memcpy(element_buffer, vector->array + idx * vector->element_size, vector->element_size);

    return VEC_SUCCESS;
}
int destroy(vec_t * vector) {
    if (vector->array == NULL)
        return VEC_ALREADY_DESTROYED;

    free(vector->array);

    return VEC_SUCCESS;
}
int sort(vec_t * vector, cmpfn cmp) {
    qsort(vector->array,vector->used_slots, vector->element_size,cmp);
    return VEC_SUCCESS;
}
