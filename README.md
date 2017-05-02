# c-vector
I wrote this for use in my school projects. A dynamic C vector is a nice thing to have in your back pocket if you need it.
It also has a thread safe version. It is also Wall clean.

# A Note On The Thread Safe Version

There are two versions of this code, which are very similar.  Documented here is the interface for the non thread safe version, 
which is found in `vec/`.  The thread safe version is found in `sync_vec` and has an almost identical interface.  The key differences
are as follows:
  * The header file is `svec.h` and the code file is `svec.c`
  * All functions have `sync_` preappended to the front, as in `sync_append` versus the non thread safe `append`
  * Similarly, all macros have `SYNC_` preappended to the front.
  * Obviously, you must link with `-lpthread`

# Basic Usage Examples

### Primative types
 
The following code creates a vector of `int`s, appends 0-499 to it, then inserts 600-798 (skipping odds) 
starting at index 300, and then removes index 100 for 100 times.  It then prints out all elements in the list, which should 
come out to 0-99 then 200-299 then 600-798 (evens only) then 300-499.  Notice how every function returns an error code, while the
actual results are copied into buffers that are pointed to by arguments.
 
```
#include <stdio.h>
#include "vec.h"


int main() {
    vec_t vector;
    int i,j;
    //doing this prevents init from thinking it already initialized the vector
    memset(&vector, 0, sizeof(int));

    //init vector and check for success
    if (init(&vector, sizeof(int)) != VEC_SUCCESS) {
        fprintf(stderr, "Could not init vector\n");
        return 1;
    }

    //add a bunch of elements to the vector, each one gets put at the end
    for (i = 0; i < 500; i++) {
        if (append(&vector, &i) != VEC_SUCCESS) {
            fprintf(stderr, "Could not append element #%d\n", i);
            //handle error here
        }
    }   

    //add a bunch of elements to the middle of the vector, moving everything 
    //over by one to make space
    for (i = 300; i < 400; i++) {
        j = i * 2;
        if (insert(&vector, &j, i) != VEC_SUCCESS) {
            fprintf(stderr, "Could not insert element into %d\n", i); 
            //handle error here
        }   
    }   

    //remove some stuff from the middle, shifting everything over to fill the empty space
    for (i = 100, j = 100; i < 200; i++) {
        if (remove_index(&vector, j) != VEC_SUCCESS) {
            fprintf(stderr, "Could not remove element from %d\n", i); 
            //handle error here
        }   
    }   

    //iterate through all vector items and print them
    for (i = 0; i < veclen(&vector); i++) {
        //get element at index i and store it in buffer j
        if (get(&vector, i, &j) == VEC_SUCCESS)
            printf("%d\n", j);
        else
            printf("Couldn't get element %d\n", i);
    }

    //free the memory allocated by the vector
    destroy(&vector);


    return 0;
}                                                                         
```

compile this with `gcc example.c vec.c` or `gcc vec.c -c; gcc example.c vec.o`, or put it in your makefile.  

### Structs

Here is very similar code as above, except using a struct. It works exactly the same.
```
#include <stdio.h>
#include "vec.h"

struct test {
    int a;
    short b;
    long c;
};

int main() {
    vec_t vector;
    int i,j;
    struct test t;
    //doing this prevents init from thinking it already initialized the vector
    memset(&vector, 0, sizeof(int));

    //init vector and check for success
    if (init(&vector, sizeof(struct test)) != VEC_SUCCESS) {
        fprintf(stderr, "Could not init vector\n");
        return 1;
    }

    //add a bunch of elements to the vector, each one gets put at the end
    for (i = 0; i < 500; i++) {
        t.a = i;
        t.b = i/2;
        t.c = 20*i;
        if (append(&vector, &t) != VEC_SUCCESS) {
            fprintf(stderr, "Could not append element #%d\n", i);
            //handle error here
        }
    }   

    //add a bunch of elements to the middle of the vector, moving everything 
    //over by one to make space
    for (i = 300; i < 400; i++) {
        t.a = i+10000;
        t.b = i+1000;
        t.c = 20*(i+1000);
        if (insert(&vector, &t, i) != VEC_SUCCESS) {
            fprintf(stderr, "Could not insert element into %d\n", i); 
            //handle error here
        }   
    }   

    //remove some stuff from the middle, shifting everything over to fill the empty space
    for (i = 100, j = 100; i < 200; i++) {
        if (remove_index(&vector, j) != VEC_SUCCESS) {
            fprintf(stderr, "Could not remove element from %d\n", i);
            //handle error here
        }
    }

    //iterate through all vector items and print them
    for (i = 0; i < veclen(&vector); i++) {
        //get element at index i and store it in buffer j
        if (get(&vector, i, &t) == VEC_SUCCESS)
            printf("%d %d %d\n", t.a, t.b, t.c);
        else
            printf("Couldn't get element %d\n", i);
    }

    //free the memory allocated by the vector
    destroy(&vector);


    return 0;
}

```


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

### int replace(vec_t * vector, void * element_ptr, int idx)

Overwrites the item at the given index with the contents of the
element_ptr buffer. Be sure to not leak memory when using this!

#### Possible return values:
  * VEC_SUCCESS
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

### VEC_ITER(vector, element_buffer, expression)

Allows you to iterate through the vector more easily. 
`vector` is a `vec_t *`, `element_buffer` is a `x *`, where `x` is the type being stored.
expression is any code you choose to execute, that will have the variable `*exlement_buffer`
availible and filled out with a given element.  `break` will work to end early.
Copies the element buffer back in to reflect any modifications when done.

#### Possible Results:
  * VEC_SUCCESS


### VEC_ITER_REMOVE(vector, element_buffer, expression)

Allows you to iterate through the vector more easily. 
`vector` is a `vec_t *`, `element_buffer` is a `x *`, where `x` is the type being stored.
expression is any code you choose to execute, that will have the variable `*exlement_buffer`
availible and filled out with a given element.  `break` will work to end early.
If the expression is true, removes the element.  If false, it copies the element buffer back in to reflect any modifications.

#### Possible Results:
  * VEC_SUCCESS

### VEC_REPLACE_BY(vector, element_buffer, element, expression)

Finds the element in the vector based on the given condition and replaces it with element.
`vector` is a `vec_t *`. `element_buffer` and `element` are `x*`, where `x` is the type that 
you are storing.  `condition` is a boolean expression involving your element_buffer.

#### Possible results:
  * VEC_SUCCESS
  * VEC_NOT_FOUND
