#include <stddef.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include "myalloc.h"

// Global variable definitions
int statusno = ERR_UNINITIALIZED;

// Memory pool variables
void* _arena_start = NULL;
size_t total_allocated_size = 0;
node_t* free_list = NULL;  // Head of the free list

int myinit(size_t size) {
    //checks to validate  the input size
    if (size <= 0 || size > MAX_ARENA_SIZE) {
        statusno = ERR_BAD_ARGUMENTS;
        return statusno;
    }

    size_t system_page_size = getpagesize(); //get the system's page size
    //Calculate how much memory is to be allocated
    total_allocated_size = (size <= system_page_size) //exactly one page is allocated if size is<=system_page_size
                          ? system_page_size // but if size is bigger, size is rouded up to allocated mem accomodates to page boundaries
                          : ((size + system_page_size - 1) / system_page_size) * system_page_size; //needed for mmap()

    //mmap() allocates a block of memory of the calculated size: total_allocated_size
    _arena_start = mmap(NULL, total_allocated_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    //PROT makes memory readable and writable
    
    //if mmap fails prints error message
    // if (_arena_start == MAP_FAILED) {
    //     perror("mmap failed"); 
    //     statusno = ERR_CALL_FAILED;
    //     return statusno;
    // }

    // Initialize the free list to the entire allocated memory block aka start of the memory pool
    free_list = (node_t*)_arena_start;
    free_list->size = total_allocated_size - sizeof(node_t); //set size of free block
    free_list->is_free = 1; //mark as free
    free_list->fwd = free_list->bwd = NULL; //fwd and bwd pointers set to NULL, the single block is set to both head and tail of list

    statusno = 0; //indicates that initalization was a success
    return total_allocated_size;
}

int mydestroy() {
    if (_arena_start != NULL) {
        munmap(_arena_start, total_allocated_size); //calls to unmap and release mem that was previously stored by mmap()
        _arena_start = NULL; //inidcate that there is no valid mem pool
        total_allocated_size = 0; //reset
        free_list = NULL;
        statusno = ERR_UNINITIALIZED;  // Reflect uninitialized state
        return 0;
    }
    statusno = ERR_UNINITIALIZED;
    return statusno;
}

void* myalloc(size_t size) {
    // check if list has been initialized
    if (free_list == NULL) { 
        statusno = ERR_UNINITIALIZED;
        return NULL;
    }

    node_t* block = free_list;
    //loops until it finds a suitable block or reaches end of list
    while (block != NULL) {
        if (block->is_free && block->size >= size) {
            // split the block if it's larger than needed
            if (block->size > size +  sizeof(node_t)) {
                node_t* new_block = (node_t*)((char*)block + sizeof(node_t) + size);
                new_block->size = block->size - size - sizeof(node_t); //set size of new block to be remaining space after splitting
                new_block->is_free = 1;
                new_block->fwd = block->fwd; //set fwd and bwd pointer of new block, links it into free list
                new_block->bwd = block;

                if (block->fwd != NULL) {
                    block->fwd->bwd = new_block;
                }
                block->fwd = new_block;
                block->size = size;
            }
            block->is_free = 0;  // Mark as allocated
            statusno = 0;  // Allocation successful
            return (void*)((char*)block + sizeof(node_t));
        }
        block = block->fwd; //move to next block if current block cannot be used for mem allocation
    }

    // No suitable block found, out of memory
    statusno = ERR_OUT_OF_MEMORY;
    return NULL;
}

void myfree(void* ptr) {
    if (ptr == NULL) return; 

    node_t* block = (node_t*)((char*)ptr - sizeof(node_t)); //calculate allocation for block
    block->is_free = 1;

    // Coalescing backward (prev block)
    if (block->bwd != NULL && block->bwd->is_free) {
        block->bwd->size += sizeof(node_t) + block->size;
        block->bwd->fwd = block->fwd;
        if (block->fwd != NULL) {
            block->fwd->bwd = block->bwd;
        }
        block = block->bwd;
    }

    // Coalescing forward (next block)
    if (block->fwd != NULL && block->fwd->is_free) {
        block->size += sizeof(node_t) + block->fwd->size;
        block->fwd = block->fwd->fwd;
        if (block->fwd != NULL) {
            block->fwd->bwd = block;
        }
    }

    statusno = 0;
}
