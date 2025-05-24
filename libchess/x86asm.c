#include "x86asm.h"

static int cnBitScanTable[64] = {
  32,  0,  1, 12,  2,  6, -1, 13,  3, -1,  7, -1, -1, -1, -1, 14,
  10,  4, -1, -1,  8, -1, -1, 25, -1, -1, -1, -1, -1, 21, 27, 15,
  31, 11,  5, -1, -1, -1, -1, -1,  9, -1, -1, 24, -1, -1, 20, 26,
  30, -1, -1, -1, -1, 23, -1, 19, 29, -1, 22, 18, 28, 17, 16, -1
};

static int BitScan(uint32_t Operand) {
  uint32_t dw = (Operand << 4) + Operand;
  dw += dw << 6;
  dw = (dw << 16) - dw;
  return cnBitScanTable[dw >> 26];
}

int Bsf(uint32_t Operand) {
#if defined(__i386__) || defined(__x86_64__)
  int eax;
  asm __volatile__ (
      "bsfl %0, %0" "\n\t"
      : "=a" (eax)
      : "0" (Operand)
      );
  return eax;
#else
  return BitScan(Operand & -Operand);
#endif
}

int Bsr(uint32_t Operand) {
#if defined(__i386__) || defined(__x86_64__)
  int eax;
  asm __volatile__ (
      "bsrl %0, %0" "\n\t"
      : "=a" (eax)
      : "0" (Operand)
      );
  return eax;
#else
  uint32_t dw = Operand | (Operand >> 1);
  dw |= dw >> 2;
  dw |= dw >> 4;
  dw |= dw >> 8;
  dw |= dw >> 16;
  return BitScan(dw - (dw >> 1));
#endif
}
