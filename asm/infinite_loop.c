//#include <stdio.h>
//#include <unistd.h>
#include <sys/types.h>
#include <asm/unistd.h>

//#include <sys/stat.h>
//#include <fcntl.h>

////////////////////
//unsigned long syscall_nr = 60;
//long exit_status = 42;

/*
  User-level applications use as integer registers for passing the sequence %rdi, %rsi, %rdx, %rcx, %r8 and %r9. The kernel interface uses %rdi, %rsi, %rdx, %r10, %r8 and %r9.
  A system-call is done via the syscall instruction. The kernel destroys registers %rcx and %r11.
  The number of the syscall has to be passed in register %rax.
  System-calls are limited to six arguments,no argument is passed directly on the stack.
  Returning from the syscall, register %rax contains the result of the system-call. A value in the range between -4095 and -1 indicates an error, it is -errno.
  Only values of class INTEGER or class MEMORY are passed to the kernel.
 */

/*

* Registers on entry:
* rax  system call number
* rcx  return address
* r11  saved rflags (note: r11 is callee-clobbered register in C ABI)
* rdi  arg0
* rsi  arg1
* rdx  arg2
* r10  arg3 (needs to be moved to rcx to conform to C ABI)
* r8   arg4
* r9   arg5
* (note: r12-r15, rbp, rbx are callee-preserved in C ABI)
*
* Only called from user space.
*
* When user can change pt_regs->foo always force IRET. That is because
* it deals with uncanonical addresses better. SYSRET has trouble
* with them due to bugs in both AMD and Intel CPUs.
*/

/*
asm ("movq %0, %%rax\n"
     "movq %1, %%rdi\n"
     "syscall"
     : // output parameters, we aren't outputting anything, no none
       // (none)
     : // input parameters mapped to %0 and %1, repsectively
       "m" (syscall_nr), "m" (exit_status)
     : // registers that we are "clobbering", unneeded since we are calling exit
       "rax", "rdi");
*/
//////////////////////
/*
#define syscall4( number, _1, _2, _3, _4 )          \
  ({                                                \
    int64_t ret;                                    \
    register int64_t r10 asm("r10") = _4;           \
    __asm__ volatile                                \
      (                                             \
       "syscall\n\t"                                \
       : "=a"( ret )                                \
       : "a"( number ),                             \
         "D"( _1 ),                                 \
         "S"( _2 ),                                 \
         "d"( _3 ),                                 \
         "r"( r10 )                                 \
       : "memory", "rcx", "r11"                     \
                                              );    \
    ret;                                            \
  })

//this is a working example of syscall3 in 32
//this is taken from the book "learning linux binary analysis
#define __syscall3(type,name,type1,arg1,type2,arg2,type3,arg3)          \
  type name(type1 arg1,type2 arg2,type3 arg3)                           \
  {                                                                     \
    long __res;                                                         \
    __asm__ volatile ("int $0x80"                                       \
                      : "=a" (__res)                                    \
                      : "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
                        "d" ((long)(arg3)),"S" ((long)(arg4)));         \
    return(type)__res;                                                  \
  }

//this is syscall3 in 64
#define _syscall3(type,name,type1,arg1,type2,arg2,type3,arg3)           \
  type name(type1 arg1,type2 arg2,type3 arg3)                           \
  {                                                                     \
    int64_t ret;                                                        \
    __asm__ volatile("syscall"                                          \
                     : "=a"( ret )                                      \
                     : "a"( __NR_##name ),                               \
                       "D"( arg1 ),                                     \
                       "S"( arg2 ),                                     \
                       "d"( arg3 )                                      \
                     : "memory", "rcx", "r11"                           \
                     );                                                 \
    return (type)ret;                                                   \
  }
*/

/*
int mywrite(int fd,char* buf, int sz){
  int c;
  int write_sys_num = 1;
  asm("movq %0 %%rax\n"
      "movq %0 %%rdi\n"
      "movq %1 %%rsi\n"
      "movq %2 %%rdx\n"
      "syscall\n"
      : "=a" (c)
      : "m" (write_sys_num), "m" (fd), "m" (buf), "m" (sz)
      : "memory", "rcx", "r11");
  return c;
}
*/


//__syscall3(ssize_t, mywrite, int, fd, const void *, buf, size_t, count);
//__syscall3(int, myopen, const char *, pathname, int, flags, mode_t, mode);
//__syscall3(ssize_t, myread, int, fd, void *, buf, size_t, count);

//_syscall3(ssize_t,write,int, fd, const void *, buf, size_t, count);
//_syscall3(int, open, const char *, pathname, int, flags, mode_t, mode);
//_syscall3(ssize_t, read, int, fd, void *, buf, size_t, count);



_start(){

  /*
  int x = 0;
  int y = 3;

  for(unsigned int i=1;i>=0;i++){
    x = 5 + i;
    y = x / 30;
    int z = y % 50;
    x = z;
  }
  */

#define mysyscall3(name,ret,arg1,arg2,arg3)     \
 __asm__ volatile("syscall"                     \
                  : "=a"( ret )                 \
                  : "a"( __NR_##name ),         \
                    "D"( arg1 ),                \
                    "S"( arg2 ),                \
                    "d"( arg3 )                 \
                  : "memory", "rcx", "r11"      \
                  );

#define mysyscall1(name,ret,arg1)                               \
 __asm__ volatile ("syscall"                                     \
                   : "=a" (ret)                                 \
                   : "a" (__NR_##name),"D" (arg1));


 /*
 int fd = open(file,0000,0);
  char buf[129];
  buf[128] = '\0';
  read(fd,buf,128);
  write(1,buf,128);
 */
 //int fd;
 int ret;
 //mysyscall3(write,ret,1,file,230);

 //mysyscall3(open,fd,file,0000,0);
 //char buf[129];
 //  buf[128] = '\0';
 //buf[127] = '\n';
 //int ret;
 //mysyscall3(read,ret,fd,buf,128);
 //mysyscall3(write,ret,1,buf,128);
  mysyscall1(exit,ret,123);

}
