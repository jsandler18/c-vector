#ifndef SVEC_H

#define SVEC_H

#define VEC_SUCCESS 0
#define VEC_INDEX_OUT_OF_BOUNDS 1 
#define VEC_COULD_NOT_ALLOCATE_MEMORY 2
#define VEC_NOT_FOUND 3
#define VEC_ALREADY_INITIALIZED 4
#define VEC_ALREADY_DESTROYED 5
#define VEC_NULL_BUFFER 6

#include <stdint.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>

typedef struct {
    uint32_t allocated_slots;
    uint32_t used_slots;
    size_t element_size;
    sem_t lock;
    void * array;
} sync_vec_t;

typedef int (*cmpfn)(const void*,const void*);

/**
 * given a pointer to a sync_vec_t struct, and the size of
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
int sync_init (sync_vec_t * vector, size_t element_size);

/**
 * appends the item pointed to by the element_ptr
 * to the end of the array. grows if needed
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 */
int sync_append(sync_vec_t * vector, void * element_ptr); 

/**
 * inserts the item pointed to by the element_ptr into
 * the given index, shifting everything else over to the left.
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 *  VEC_INDEX_OUT_OF_BOUNDS
 */
int sync_insert(sync_vec_t * vector, void * element_ptr, int idx);

/**
 * overwrites the item at the given index with the contents of the
 * element_ptr buffer. Be sure to not leak memory when using this!
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_INDEX_OUT_OF_BOUNDS
 */
int sync_replace(sync_vec_t * vector, void * element_ptr, int idx);

/**
 * returns the current length of the vector
 */
int sync_veclen(sync_vec_t * vector);

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
int sync_remove_element(sync_vec_t * vector, void * element_ptr);

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
int sync_remove_index(sync_vec_t * vector, int idx);

/**
 * gets the item at the given index and copies it into 
 * the memory pointed to by element_buffer
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_INDEX_OUT_OF_BOUNDS
 *  VEC_NULL_BUFFER
 */
int sync_get(sync_vec_t * vector, int idx, void * element_buffer);

/**
 * frees all memory given to this vector
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_ALREADY_DESTROYED
 */
int sync_destroy(sync_vec_t * vector);

/**
 * sorts the array in place. Always succeeds.
 *
 * possible return values:
 *  VEC_SUCCESS
 */
int sync_sort(sync_vec_t * vector,cmpfn cmp);

/**
 * creates a copy of the vector. The source vector should have already been
 * initialized, while the destination vector should already be allocated but
 * not initalized
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 *  VEC_ALREADY_INITIALIZED
 *
 */
int sync_copy(sync_vec_t * srcvec, sync_vec_t * dstvec);

/**
 * creates a copy of the vector's internal array, and sets resultptr to be the location 
 * of the pointer to the array.  The allocated space is just a regular dynamic array and
 * should be freed using free(*resultptr) when done.
 *
 * possible return values:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 */
int sync_to_array(sync_vec_t * vec, void ** resultptr);

/**
 * not intended for use outside of macros. this version of remove does not take the lock so its caller can take the lock for it.
 * This means the caller macro can hold the lock and call remove without deadlocking itself
 */
int remove_index(sync_vec_t * vector, int idx);

/**
 * finds the element in the vector based on the given condition.
 * vector is a sync_vec_t *. element_buffer is a x*, where x is the type that 
 * you are storing.  condition is a boolean expression involving your element_buffer.
 *
 * Example:
 *      struct x {
 *          int y;
 *          int z;
 *      }
 *      
 *      struct x buf;
 *      VEC_FIND_BY(vector, &buf, buf.y == 1)
 *
 * possible results:
 *  VEC_SUCCESS
 *  VEC_NOT_FOUND
 */
#define SYNC_VEC_FIND_BY(vector, element_buffer, condition) ({\
        sem_wait(&(vector)->lock);\
        int _ret = VEC_NOT_FOUND; \
        unsigned int _i;\
        for (_i = 0; _i < (vector)->used_slots; _i++) { \
            memcpy(element_buffer, (vector)->array + _i * (vector)->element_size, (vector)->element_size); \
            if (condition) { \
                _ret = VEC_SUCCESS; \
                break; \
            } \
        } \
        sem_post(&(vector)->lock);\
        _ret; \
})

/**
 * finds the element in the vector based on the given condition and removes it.
 * vector is a sync_vec_t *. element_buffer is a x*, where x is the type that 
 * you are storing.  condition is a boolean expression involving your element_buffer.
 *
 * possible results:
 *  VEC_SUCCESS
 *  VEC_NOT_FOUND
 */
#define SYNC_VEC_REMOVE_BY(vector, element_buffer, condition) ({\
        sem_wait(&(vector)->lock);\
        int _ret = VEC_NOT_FOUND; \
        unsigned int _i;\
            for (_i = 0; _i < (vector)->used_slots; _i++) { \
                memcpy(element_buffer, (vector)->array + _i * (vector)->element_size, (vector)->element_size); \
                if (condition) { \
                    _ret = VEC_SUCCESS; \
                    remove_index(vector, _i); \
                    break; \
                } \
        } \
        sem_post(&(vector)->lock);\
        _ret; \
})

/**
 * creates a copy of the first vector and stores it in the second vector.
 * The copy keeps only the elements that satisfy the condition.
 * srcvector is an initialized sync_vec_t * , dstvector is an uninitailized sync_vec_t *. 
 * element_buffer is a x*, where x is the type that you are storing. 
 * condition is a boolean expression involving your element_buffer.
 *
 * possible results:
 *  VEC_SUCCESS
 */
#define SYNC_VEC_SELECT(srcvector, dstvector, element_buffer, condition) ({\
        sem_wait(&(srcvector)->lock);\
        unsigned int _i;\
        copy(srcvector, dstvector); \
        for (_i = 0; _i < (dstvector)->used_slots; _i++) {\
            memcpy(element_buffer, (dstvector)->array + _i * (dstvector)->element_size, (dstvector)->element_size); \
            if (!(condition)) { \
                remove_index(dstvector, _i); \
                _i--;\
            }\
        }\
        sem_post(&(srcvector)->lock);\
        VEC_SUCCESS;\
})

/**
 * The removes the elements that do not satisfy the condition.
 * vector is an initialized sync_vec_t *. 
 * element_buffer is a x*, where x is the type that you are storing. 
 * condition is a boolean expression involving your element_buffer.
 *
 * possible results:
 *  VEC_SUCCESS
 */
#define SYNC_VEC_FILTER(vector, element_buffer, condition) ({\
        sem_wait(&(vector)->lock);\
        unsigned int _i;\
        for (_i = 0; _i < (vector)->used_slots; _i++) {\
            memcpy(element_buffer, (vector)->array + _i * (vector)->element_size, (vector)->element_size); \
            if (!(condition)) { \
                remove_index(vector, _i); \
                _i--;\
            }\
        }\
        sem_post(&(vector)->lock);\
        VEC_SUCCESS;\
})

/**
 * creates a new vector and places it in dstvector
 * The new vector maps all elements from the first.
 * srcvector is an initialized sync_vec_t * , dstvector is an uninitailized sync_vec_t *. 
 * mapped_ele_size is a size_t that is the size of the elements in the mapped vector;
 * src_element_buffer is a x*, where x is the type that you are storing. 
 * dst_element_buffer is a y*, where y is the type of the mapped elements
 * expression is a series of statements that fills out dst_element_buffer.
 *
 * Example:
 *      struct x {
 *          int y;
 *          int z;
 *      }
 *      struct w {
 *          char a;
 *          long b;
 *      }
 *      
 *      struct x buf;
 *      struct w buf2;
 *      VEC_MAP(vector,dstvec,sizeof(struct w), &buf, &buf2, buf2.a = buf1.y % 128; buf2.b = 100 * buf1.z;)
 *
 * possible results:
 *  VEC_SUCCESS
 *  VEC_COULD_NOT_ALLOCATE_MEMORY
 *  VEC_ALREADY_INITIALIZED
 */
#define SYNC_VEC_MAP(srcvector, dstvector, mapped_ele_size, src_element_buffer, dst_element_buffer, expression) ({\
        sem_wait(&(srcvector)->lock);\
        int _ret = VEC_SUCCESS; \
        unsigned int _i;\
        if ((dstvector)->array != NULL && (dstvector)->allocated_slots > 0)  \
            _ret = VEC_ALREADY_INITIALIZED; \
        else {\
            (dstvector)->element_size = mapped_ele_size; \
            (dstvector)->used_slots = (srcvector)->used_slots;\
            (dstvector)->allocated_slots = (srcvector)->allocated_slots;\
            (dstvector)->array = calloc((srcvector)->allocated_slots, mapped_ele_size);\
            if ((dstvector)->array == NULL) \
                _ret = VEC_COULD_NOT_ALLOCATE_MEMORY; \
            else { \
                for (_i = 0; _i < (srcvector)->used_slots; _i++) {\
                    memcpy(src_element_buffer, (srcvector)->array + _i * (srcvector)->element_size, (srcvector)->element_size); \
                    expression\
                    memcpy((dstvector)->array + _i * (dstvector)->element_size, dst_element_buffer, (dstvector)->element_size); \
                }\
            } \
        }\
        sem_post(&(srcvector)->lock);\
        _ret;\
})
#endif


/**
 * Allows you to iterate through the vector more easily. 
 * vector is a sync_vec_t *, element_buffer is a x *, where x is the type being stored.
 * expression is any code you choose to execute, that will have the variable *exlement_buffer
 * availible and filled out with a given element.  break will work to end early.
 *
 * possible results:
 *  VEC_SUCCESS
 */
#define SYNC_VEC_ITER(vector, element_buffer, expression) ({\
        sem_wait(&(vector)->lock);\
        int _ret = VEC_SUCCESS;\
        unsigned int _i;\
        for (_i = 0; _i < (vector)->used_slots; _i++) {\
            memcpy(element_buffer, (vector)->array + _i * (vector)->element_size, (vector)->element_size);\
            expression;\
        }\
        sem_post(&(vector)->lock);\
        _ret;\
})

