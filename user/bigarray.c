#include "kernel/types.h"
#include "user/user.h"

#define ARRAY_SIZE (1 << 16) // 2^16 = 65536
#define NUM_CHILDREN 4

int main(int argc, char *argv[]) {

  int *arr = malloc(ARRAY_SIZE * sizeof(int));
  if (arr == 0) {
    printf("Error: Failed to allocate memory for array\n");
    exit(1, "Memory allocation failed");
  }

    // Initialize the array
    for (int i = 0; i < ARRAY_SIZE; i++) {
      arr[i] = i;
    }

  int pids[NUM_CHILDREN];
  int statuses[NUM_CHILDREN];
  long long sum = 0;

  // Create child processes using forkn
  int id = forkn(NUM_CHILDREN, pids);
  if (id < 0) {
    exit(1, "forkn failed");
    return 1;
  }

  // Parent process
  else if(id == 0){
    printf("\nParent: Created child processes with PIDs: ");
    for (int i = 0; i < NUM_CHILDREN; i++) {
      printf("%d ", pids[i]);
    }
    printf("\n");
  }

  // Determine the range of the array to process
  // Child process
  else{
    int start = (id - 1) * (ARRAY_SIZE / NUM_CHILDREN);
    int end = start + (ARRAY_SIZE / NUM_CHILDREN);
    long long partial_sum = 0;

    for (int i = start; i < end; i++) {
      partial_sum += arr[i];
    }

    exit(partial_sum, ""); // Exit with the partial sum as the status
  }

  // Wait for all child processes to finish
  int finished_children;
  if (waitall(&finished_children, statuses) < 0) {
    exit(1, "waitall failed");
  }

  // Verify the number of finished children matches the number created
  if (finished_children != NUM_CHILDREN) {
    printf("Error: Expected %d children, but %d finished.\n", NUM_CHILDREN, finished_children);
    exit(1, "");
  }

  // Calculate the total sum
  for (int i = 0; i < finished_children; i++) {
    sum += statuses[i];
    printf("Partial sum of child %d = %d\n", i + 1, statuses[i]);
  }

  printf("Parent: Total sum = %d\n", sum);

  free(arr);
  exit(0, "Parent finished");
  return 0;
}