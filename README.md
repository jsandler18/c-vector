# c-vector
I wrote this for school projects. Feel free to use it

# Functions

### int init (vec_t * vector, size_t element_size)

Given a vector, and the size of the elements that will be stored in the vector,
initializes the vector with some memory. The vector must
have been zeroed out first, or may trigger already initialized error
 
#### Possible return values:

  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY
  * VEC_ALREADY_INITIALIZED
 
### int append(vec_t * vector, void * element_ptr)

Appends a copy of the contents pointed to by element_ptr to the end of the vector.

#### Possible return values:

  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY
  
### int insert(vec_t * vector, void * element_ptr, int idx)

Inserts a copy of the contents pointed to by the element_ptr into the vector at
the given index.  This operation shifts everything else over to make room for the new element.

####Possible return values:

  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY
  * VEC_INDEX_OUT_OF_BOUNDS

### int veclen(vec_t * vector)

Returns the current length of the vector. Cannot fail.

### int remove_element(vec_t * vector, void * element_ptr)

Removes the first item in the vector whose bytes are equivalent to the bytes 
pointed to by element_ptr.  This operation shifts everything over to fill the empty gap.
 
#### Possible return values:
  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY
  * VEC_NULL_BUFFER
  * VEC_NOT_FOUND

### int remove_index(vec_t * vector, int idx)

Removes the item that is at the given index. This operation shifts everything over to fill the empty gap.
 
#### Possible return values:
  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY
  * VEC_INDEX_OUT_OF_BOUNDS

### int get(vec_t * vector, int idx, void * element_buffer)

Gets the item at the given index and copies it into 
the memory pointed to by element_buffer.

#### Possible return values:
  * VEC_SUCCESS
  * VEC_INDEX_OUT_OF_BOUNDS
  * VEC_NULL_BUFFER
 
### int destroy(vec_t * vector)

Frees all memory given to this vector. Zeros out the vector for potential reuse.
 
#### Possible return values:
  * VEC_SUCCESS
  * VEC_ALREADY_DESTROYED


### int sort(vec_t * vector,cmpfn cmp)

Sorts the array in place using the given comparison function. Always succeeds.
the comparison function is defined as ```typedef int (*cmpfn)(const void*,const void*);```

#### Possible return values:
  * VEC_SUCCESS
  
### int copy(vec_t * srcvec, vec_t * dstvec)

Creates a copy of the vector. The source vector should have already been
initialized, while the destination vector should already be allocated but
not initalized.

#### Possible return values:
  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY
  * VEC_ALREADY_INITIALIZED


