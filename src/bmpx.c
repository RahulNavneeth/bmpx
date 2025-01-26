#include "bmpx.h"
#include <stdio.h>
#include <stdlib.h>

const char tag[] = {'B', 'M'};

BmpX *init_bmpx(int32_t width, int32_t height) {
  BmpX *new_bmpx = (BmpX *)malloc(sizeof(BmpX));
  if (new_bmpx == NULL) {
    fprintf(stderr, "ERR: memory allocation for BmpX failed\n");
    return NULL;
  }

  int32_t *pixels = (int32_t *)calloc(width * height, sizeof(int32_t));
  if (pixels == NULL) {
    fprintf(stderr, "ERR: memory allocation for pixels failed\n");
    free(new_bmpx);
    return NULL;
  }

  new_bmpx->width = width;
  new_bmpx->height = height;
  new_bmpx->pixels = pixels;

  for (int i = 0; i < width * height; i++) {
    new_bmpx->pixels[i] = 0xFFFFFF;
  }

  int centerX = width / 2;
  int centerY = height / 2;
  int radius = width / 3;

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      int dx = x - centerX;
      int dy = y - centerY;
      if (dx * dx + dy * dy <= radius * radius) {
        new_bmpx->pixels[y * width + x] = 0x0000FF; // Red in little-endian
      }
    }
  }

  printf("BMPX init - Width: %d, Height: %d\n", new_bmpx->width,
         new_bmpx->height);
  return new_bmpx;
}

int *init_header(BmpX *bmpx) {
  int row_size = bmpx->width * 3;
  int padding = (4 - (row_size % 4)) % 4;
  int data_size = (row_size + padding) * bmpx->height;
  int file_size = 54 + data_size;

  int *header = (int *)malloc(sizeof(int) * 12);
  if (header == NULL) {
    fprintf(stderr, "ERR: memory allocation for header failed\n");
    return NULL;
  }

  header[0] = file_size;    /* Total file size */
  header[1] = 0x00;         /* Unused */
  header[2] = 0x36;         /* Pixel data offset (54 bytes header) */
  header[3] = 0x28;         /* Header size (40 bytes) */
  header[4] = bmpx->width;  /* Image width */
  header[5] = bmpx->height; /* Image height */
  header[6] = 0x180001;     /* 24 bits per pixel, 1 color plane */
  header[7] = 0;            /* No compression */
  header[8] = data_size;    /* Pixel data size */
  header[9] = 0x002e23;     /* Horizontal resolution */
  header[10] = 0x002e23;    /* Vertical resolution */
  header[11] = 0;           /* No color palette */

  return header;
}

void write_bmpx(BmpX *bmpx, char *file_path) {
  FILE *fp = fopen(file_path, "wb");
  if (fp == NULL) {
    fprintf(stderr, "ERR: unable to open the file \'%s\' for writing\n",
            file_path);
    return;
  }

  fwrite(&tag, sizeof(tag), 1, fp);

  int *header = init_header(bmpx);
  if (header == NULL) {
    fclose(fp);
    return;
  }

  fwrite(header, sizeof(int), 12, fp);

  int row_size = bmpx->width * 3;
  int padding = (4 - (row_size % 4)) % 4;

  for (int y = bmpx->height - 1; y >= 0; --y) {
    for (int x = 0; x < bmpx->width; ++x) {
      int pixel = bmpx->pixels[y * bmpx->width + x];
      unsigned char blue = (pixel & 0xFF);
      unsigned char green = (pixel >> 8) & 0xFF;
      unsigned char red = (pixel >> 16) & 0xFF;
      unsigned char color[3] = {blue, green, red}; // Little Endian
      fwrite(color, sizeof(unsigned char), 3, fp);
    }
    unsigned char padding_bytes[3] = {0};
    fwrite(padding_bytes, sizeof(unsigned char), padding, fp);
  }

  fclose(fp);
  free(header);
}

void free_bmpx(BmpX *bmpx) {
  if (bmpx != NULL) {
    free(bmpx->pixels);
    free(bmpx);
  }
}
