
/*
static __attribute__((used)) int var2 = 3;
void func2(int parm)
{
  // Register input - volatile because has no outputs, writes to memory
  asm volatile("mov %0, var2" : : "r" (parm) : "memory");
}
*/
void func1(int parm)
{
  /* Register input - volatile because has no outputs, writes to memory */
  //asm volatile("mov %0, var2" : : "r" (parm) : "memory");
}

_start/*int main*/(void)
{
  int x = 10, y=9, z;

  asm ("movl %1, %%eax;"
         "movl %%eax, %0;"
       //GCC is free here to allocate any register when the "r" constraint is used

       :"=r"(z)    /* y is output operand */
        :"r"(x)     /* x is input operand */
         //And since %eax is specified in the clobbered list, GCC doesn't use it anywhere else to store data
       :"%eax");   /* %eax is clobbered register */
}
/*
_start(){

  func1(1);
}
*/
