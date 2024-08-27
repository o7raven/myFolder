#include "hex.h"
int checkHex(uint64_t n)
{
  uint64_t swapped = bswap_64(n);
  printf("original: 0x%016"PRIx64"\nSwapped: 0x%016"PRIx64"\n", n, swapped);
  return EXIT_SUCCESS;
}
