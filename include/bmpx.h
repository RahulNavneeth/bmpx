#ifndef BMPX_H
#define BMPX_H

#include <stdio.h>

typedef struct {
  int32_t width, height, *pixels;
} BmpX;

extern const char tag[];

BmpX *init_bmpx(int32_t width, int32_t height);
void write_bmpx(BmpX *bmpx, char *file_path);
void free_bmpx(BmpX *bmpx);
int *init_header(BmpX *bmpx);

#endif
