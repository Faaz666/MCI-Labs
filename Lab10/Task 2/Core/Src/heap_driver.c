#include "heap_driver.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

extern void print_uart(const char* msg);

#define HEAP_START_ADDR  ((uint8_t*)0x20001000)
#define HEAP_SIZE        (4 * 1024)
#define BLOCK_SIZE       16
#define BLOCK_COUNT      (HEAP_SIZE / BLOCK_SIZE)

// Students should be provided the above code (includes and defines) and the function declarations in this file.
// They can figure out the rest.

// Allocation bitmap: 0 = free, 1 = used

// Add you code below
uint8_t block_map[BLOCK_COUNT];
void heap_init(void) {
    for (int i = 0; i < BLOCK_COUNT; i++) {
        block_map[i] = 0; // Mark all blocks as free
    }
    print_uart("Heap initialized\r\n");
}

void* heap_alloc(size_t size) {
    char buffer[100];
    int blocks = 0;
    if (size % BLOCK_SIZE != 0) {
        blocks = (size / BLOCK_SIZE) + 1;
    } else {
        blocks = size / BLOCK_SIZE;
    }
    
    snprintf(buffer, sizeof(buffer), "Allocating %u bytes (%d blocks)\r\n", (unsigned int)size, blocks);
    print_uart(buffer);
    
    for (int i = 0; i < BLOCK_COUNT; i++) {
        if (block_map[i] == 0) {
            int count = 1;
            int j;
            for (j = i + 1; j < BLOCK_COUNT && block_map[j] == 0; j++) {
                count++;
            }
            if (count >= blocks) {
                for (int k = i; k < i + blocks; k++) {
                    block_map[k] = 1;
                    snprintf(buffer, sizeof(buffer), "Allocated block %d\r\n", k);
                    print_uart(buffer);
                }
                void* ptr = (void*)(HEAP_START_ADDR + i * BLOCK_SIZE);
                snprintf(buffer, sizeof(buffer), "Allocated at 0x%08X\r\n", (unsigned int)ptr);
                print_uart(buffer);
                return ptr;
            }
        }
    }
    print_uart("Allocation failed as could not find any contiguous free blocks\r\n");
    return NULL;
}

void heap_free(void* ptr) {
    char buffer[100];
    
    if (ptr == NULL) {
        return;
    }
    
    snprintf(buffer, sizeof(buffer), "Freeing pointer 0x%08X\r\n", (unsigned int)ptr);
    print_uart(buffer);
    
    uint8_t* byte_ptr = (uint8_t*)ptr;
    if (byte_ptr < HEAP_START_ADDR || byte_ptr >= HEAP_START_ADDR + HEAP_SIZE) {
        print_uart("error: out of heap bounds\r\n");
        return;
    }
    
    if ((byte_ptr - HEAP_START_ADDR) % BLOCK_SIZE != 0) {
        print_uart("error: pointer not aligned to block boundary\r\n");
        return;
    }
    
    int block = (byte_ptr - HEAP_START_ADDR) / BLOCK_SIZE;
    int freed_count = 0;
    for (int i = block; i < BLOCK_COUNT && block_map[i] == 1; i++) {
        block_map[i] = 0;
        snprintf(buffer, sizeof(buffer), "Freed block %d\r\n", i);
        print_uart(buffer);
        freed_count++;
    }
    snprintf(buffer, sizeof(buffer), "Total blocks freed: %d\r\n", freed_count);
    print_uart(buffer);
}