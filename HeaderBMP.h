#ifndef CLIONPROJECTS_HEADERBMP_H
#define CLIONPROJECTS_HEADERBMP_H

#include <cstdio>
#include <cstdint>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <vector>
#include <fstream>
#include <cmath>
#include <map>

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER;
#pragma pack(pop)

typedef struct tagRGBTRIPLE {
    BYTE rgbBlue;
    BYTE rgbGreen;
    BYTE rgbRed;
} RGBTRIPLE;

#endif //CLIONPROJECTS_HEADERBMP_H
