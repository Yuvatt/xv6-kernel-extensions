#include "kernel/types.h"
#include "user/user.h"

#define ARRAY_SIZE (1 << 16) // 2^16 = 65536
#define MAX_CHILDREN 16

int main(int argc, char *argv[]) {

  int n = 4; // Number of child processes
  if (argc > 1) {
    n = atoi(argv[1]); // Allow the user to specify the number of children
    if (n < 1 || n > MAX_CHILDREN) {
      printf("Invalid number of child processes. Must be between 1 and %d.\n", MAX_CHILDREN);
      exit(1, "Invalid input");
    }
  }

  int *arr = malloc(ARRAY_SIZE * sizeof(int));
  if (arr == 0) {
    printf("Error: Failed to allocate memory for array\n");
    exit(1, "Memory allocation failed");
  }
  int pids[MAX_CHILDREN];
  int statuses[MAX_CHILDREN];
  int sum = 0;

  // Initialize the array with consecutive integers
  for (int i = 0; i < ARRAY_SIZE; i++) {
    arr[i] = i;
  }

  printf("Debug: Calling forkn with n=%d\n", n);
  // Create child processes using forkn
  int id = forkn(n, pids);
  if (id < 0) {
    printf("forkn failed\n");
    exit(1, "forkn failed");
  }

  printf("Debug: forkn succeeded, PIDs: ");
  // Determine the range of the array to process
  if (id > 0) {
    // Child process
    int start = (id - 1) * (ARRAY_SIZE / n);
    int end = start + (ARRAY_SIZE / n);
    int partial_sum = 0;

    for (int i = start; i < end; i++) {
      partial_sum += arr[i];
    }

    printf("Child %d (PID %d): Partial sum = %d\n", id, getpid(), partial_sum);
    exit(partial_sum, ""); // Exit with the partial sum as the status
  }

  // Parent process
  printf("Parent: Created child processes with PIDs: ");
  for (int i = 0; i < n; i++) {
    printf("%d ", pids[i]);
  }
  printf("\n");

  // Wait for all child processes to finish
  int finished_children;
  if (waitall(&finished_children, statuses) < 0) {
    printf("waitall failed\n");
    exit(1, "waitall failed");
  }

  // Verify the number of finished children matches the number created
  if (finished_children != n) {
    printf("Error: Expected %d children, but %d finished.\n", n, finished_children);
    exit(1, "Mismatch in child count");
  }

  // Calculate the total sum
  for (int i = 0; i < finished_children; i++) {
    sum += statuses[i];
  }

  printf("Parent: Total sum = %d\n", sum);

  // Verify the result
//   int expected_sum = (ARRAY_SIZE * (ARRAY_SIZE - 1)) / 2; // Sum of 0 to ARRAY_SIZE-1
//   if (sum == expected_sum) {
//     printf("Success: Total sum matches expected value (%d).\n", expected_sum);
//   } else {
//     printf("Error: Total sum (%d) does not match expected value (%d).\n", sum, expected_sum);
//   }

  free(arr);
  exit(0, "Parent finished");
}