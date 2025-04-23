#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int status;
  char exit_msg[32];

  // Get the status argument
  argint(0, &status);

  // Get the exit message from userspace
  if (argstr(1, exit_msg, sizeof(exit_msg)) < 0){
    printf("sys_exit: argstr failed\n");
    return -1;
  }

  // Save the exit message in the PCB
  struct proc *p = myproc();
  safestrcpy(p->exit_msg, exit_msg, sizeof(p->exit_msg));

  // Call the kernel's exit function
  exit(status, exit_msg);
  return 0; // Not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 status_addr, msg_addr;

  // Get the status pointer from userspace
  argaddr(0, &status_addr);

  // Get the exit message pointer from userspace
  argaddr(1, &msg_addr);

  // Call the kernel's wait function
  int status;
  char exit_msg[32];
  int pid = wait(&status, exit_msg);

  // Copy the status and exit message to userspace
  if (pid >= 0) {
      if (copyout(myproc()->pagetable, status_addr, (char *)&status, sizeof(status)) < 0)
          return -1;
      if (copyout(myproc()->pagetable, msg_addr, exit_msg, sizeof(exit_msg)) < 0)
          return -1;
  }

  return pid;
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

//This system call returns the size of the
//running process’ memory in bytes.
uint64
sys_memsize(void)
{
  return myproc()->sz;
}
