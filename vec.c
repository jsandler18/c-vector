#include <stdlib.h>
#include <string.h>
#include "vec.h"

#define MIN_SIZE 64

int grow(vec_t * vector);

int init (vec_t * vector, size_t element_size) {
    vector->element_size = element_size;
    vector->allocated_slots = MIN_SIZE;
    vector->used_slots = 0;
    //check already initialized
    if (vector->array != NULL && vector->allocated_slots > 0) 
        return VEC_ALREADY_INITIALIZED;

    vector->array = calloc(MIN_SIZE, element_size);

    //check memory allocation
    if (vector->array == NULL) 
        return VEC_COULD_NOT_ALLOCATE_MEMORY;

    //everything is good
    return VEC_SUCCESS;
}
int insert(vec_t * vector, void * element_ptr, int idx);
int append(vec_t * vector, void * element_ptr); 
int veclen(vec_t * vector);
int remove_element(vec_t * vector, void * element_ptr);
int remove_index(vec_t * vector, int idx);
int get(vec_t * vector, int idx, void * element_buffer);
int destroy(vec_t * vector);
