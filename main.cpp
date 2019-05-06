#include "BMP.h"

int main(){
    BMP bmp;
    bmp.loadBMP("D:\\CLionProjects\\barbara.bmp");


    bmp.loadBMP("D:\\CLionProjects\\barbara.bmp");
    bmp.saveBMP("D:\\CLionProjects\\copyBMP.bmp");//проверка

    bmp.onlyOneColor();

    bmp.correlation();

    bmp.RGBtoYCbCr();

    bmp.YCbCrToRGB();

    bmp.decimation(4);

    bmp.frequency();

    bmp.entropy();

     bmp.DPCM(false);//false - entopy, true - save histogram

    bmp.decimation(4);

    bmp.entropy();
    bmp.frequency();
    bmp.DPCM(false);

    bmp.bitPlane();
    bmp.autocor();

    return 0;
}
