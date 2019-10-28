#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <string>
#include "functions.h"

using std::string;

struct Pixel {
  int r; // red
  int g; // green
  int b; // blue
};

Pixel** createImage(int width, int height);
void deleteImage(Pixel** image, int width);
int* createSeam(int length);
void deleteSeam(int* seam);
bool loadImage(string filename, Pixel** image, int width, int height);
bool outputImage(string filename, Pixel** image, int width, int height);
int energy(Pixel** image, int x, int y, int width, int height);
bool isValidDigit(string str);
int loadVerticalSeam(Pixel** image, int start_col, int width, int height, int* seam);
int loadHorizontalSeam(Pixel** image, int start_row, int width, int height, int* seam);
int* findMinVerticalSeam(Pixel** image, int width, int height);
int* findMinHorizontalSeam(Pixel** image, int width, int height);
void removeVerticalSeam(Pixel** image, int width, int height, int* verticalSeam);
void removeHorizontalSeam(Pixel** image, int width, int height, int* horizontalSeam);

#endif
