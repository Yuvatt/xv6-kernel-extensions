#include "kernel/types.h"
#include "user/user.h"

int main() {
    // (a) Print the initial memory size
    printf("Initial memory size: %d bytes\n", memsize());

    // (b) Allocate 20k more bytes of memory
    char *allocated_memory = malloc(20000); // 20 KB
    if (allocated_memory == 0) {
        printf("Memory allocation failed\n");
        exit(1, "");
    }

    // (c) Print memory size after allocation
    printf("Memory size after allocation: %d bytes\n", memsize());

    // (d) Free the allocated memory
    free(allocated_memory);

    // (e) Print memory size after releasing memory
    printf("Memory size after release: %d bytes\n", memsize());

    exit(0, "");
}