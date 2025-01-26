#include "bmpx.h"
#include <stdio.h>
#include <stdlib.h>

int32_t main() {
  BmpX *bmpx = init_bmpx(1500, 1000);
  write_bmpx(bmpx, "../assets/japan??.bmp");
  free_bmpx(bmpx);
  return 0;
}
