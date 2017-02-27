#ifndef VEC_H

#define VEC_H

#define VEC_SUCCESS 0
#define VEC_INDEX_OUT_OF_BOUNDS 1 
#define VEC_COULD_NOT_ALLOCATE_MEMORY 2
#define VEC_NOT_FOUND 3
#define VEC_ALREADY_INITIALIZED 4
#define VEC_ALREADY_DESTROYED 5
#define VEC_NULL_BUFFER 6

#include <stdint.h>
#include <stdio.h>

typedef struct {
    uint32_t allocated_slots;
    uint32_t used_slots;
    size_t element_size;
    void * array;
} vec_t;

typedef int (*cmpfn)(const void*,const void*);

/**
 * given a pointer to a vec_t struct, and the size of
 * the elements that will be stored in the vector,
 * initializes the vector with some memory. vector must
 * have been zeroed out first, or may trigger already initialized error
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 *  VEC_ALREADY_INITIALIZED
 *
 */
int init (vec_t * vector, size_t element_size);

/**
 * appends the item pointed to by the element_ptr
 * to the end of the array. grows if needed
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 */
int append(vec_t * vector, void * element_ptr); 

/**
 * inserts the item pointed to by the element_ptr into
 * the given index, shifting everything else over to the left.
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 *  VEC_INDEX_OUT_OF_BOUNDS
 */
int insert(vec_t * vector, void * element_ptr, int idx);

/**
 * returns the current length of the vector
 */
int veclen(vec_t * vector);

/**
 * removes the given item that is equivalent to the 
 * element pointed to by element_ptr and shifts everything
 * else over to the left. Shrinks the array
 * if less than 1/4 of the allocated space is in use.
 * 
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 *  VEC_NULL_BUFFER
 *  VEC_NOT_FOUND
 */
int remove_element(vec_t * vector, void * element_ptr);

/**
 * removes the given item that is at the given index
 * and shifts everything else over to the left. Shrinks the array
 * if less than 1/4 of the allocated space is in use.
 * 
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 *  VEC_INDEX_OUT_OF_BOUNDS
 */
int remove_index(vec_t * vector, int idx);

/**
 * gets the item at the given index and copies it into 
 * the memory pointed to by element_buffer
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_INDEX_OUT_OF_BOUNDS
 *  VEC_NULL_BUFFER
 */
int get(vec_t * vector, int idx, void * element_buffer);

/**
 * frees all memory given to this vector
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_ALREADY_DESTROYED
 */
int destroy(vec_t * vector);

/**
 * sorts the array in place. Always succeeds.
 *
 * possible return values:
 *  VEC_SUCCESS
 */
int sort(vec_t * vector,cmpfn cmp);
#endif
