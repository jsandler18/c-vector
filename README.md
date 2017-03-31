# c-vector
I wrote this for use in my school projects. A dynamic C vector is a nice thing to have in your back pocket if you need it.
Feel free to use it

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

#### Possible return values:

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
the comparison function is defined as 

`typedef int (*cmpfn)(const void*,const void*);`

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


### int to_array(vec_t * vec, void ** resultptr)

Creates a copy of the vector's internal array, and sets resultptr to be the location 
of the pointer to the array.  The allocated space is just a regular dynamic array and
should be freed using free(*resultptr) when done.

#### Possible return values:
  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY

# Macros

### VEC_FIND_BY(vector, element_buffer, condition)

Finds the element in the vector based on the given condition and stores it in `element_buffer`.
`vector` is a `vec_t *`. `element_buffer` is a `x*`, where `x` is the type that 
you are storing.  `condition` is a boolean expression involving your element_buffer.

#### Example:
```
     struct x {
         int y;
         int z;
     }
     
     struct x buf;
     VEC_FIND_BY(vector, &buf, buf.y == 1)
 ```
     
#### Possible results:
  * VEC_SUCCESS
  * VEC_NOT_FOUND
 
### VEC_REMOVE_BY(vector, element_buffer, condition) 

Finds the element in the vector based on the given condition and removes it.
`vector` is a `vec_t *`. `element_buffer` is a `x*`, where `x` is the type that 
you are storing.  `condition` is a boolean expression involving your element_buffer.

#### Possible results:
  * VEC_SUCCESS
  * VEC_NOT_FOUND



#### VEC_SELECT(srcvector, dstvector, element_buffer, condition) 

Creates a copy of the first vector and stores it in the second vector.
The copy keeps only the elements that satisfy the condition.
`srcvector` is an initialized `vec_t *` , `dstvector` is an uninitailized `vec_t *`. 
`element_buffer` is a `x*`, where `x` is the type that you are storing. 
`condition` is a boolean expression involving your element_buffer.

#### Possible results:
  * VEC_SUCCESS

### VEC_FILTER(vector, element_buffer, condition) 

Removes the elements that do not satisfy the condition.
`vector` is an initialized `vec_t *`. 
`element_buffer` is a `x*`, where `x` is the type that you are storing. 
`condition` is a boolean expression involving your element_buffer.

#### Possible results:
 * VEC_SUCCESS

### VEC_MAP(srcvector, dstvector, mapped_ele_size, src_element_buffer, dst_element_buffer, expression) 

Creates a new vector and places it in `dstvector`.
The new vector maps all elements from the first.
`srcvector` is an initialized `vec_t *` , `dstvector` is an uninitailized `vec_t *`. 
`mapped_ele_size` is a `size_t` that is the size of the elements in the mapped vector;
`src_element_buffer` is a `x*`, where `x` is the type that you are storing. 
`dst_element_buffer` is a `y*`, where `y` is the type of the mapped elements
`expression` is a series of statements that fills out `dst_element_buffer`.

#### Example:
```
     struct x {
         int y;
         int z;
     }
     struct w {
         char a;
         long b;
     }
     
     struct x buf;
     struct w buf2;
     VEC_MAP(vector,dstvec,sizeof(struct w), &buf, &buf2, buf2.a = buf1.y % 128; buf2.b = 100 * buf1.z;)
```
#### Possible results:
  * VEC_SUCCESS
  * VEC_COULD_NOT_ALLOCATE_MEMORY
  * VEC_ALREADY_INITIALIZED

